template<typename FieldT>
example_gadget<FieldT>::example_gadget(protoboard<FieldT> &pb) :
        gadget<FieldT>(pb, FMT(annotation_prefix, " example_gadget"))
{
    // Allocate space for the verifier input.
    const size_t input_size_in_bits = sha256_digest_len;
    {
        const size_t input_size_in_field_elements = div_ceil(input_size_in_bits, FieldT::capacity());
        input_as_field_elements.allocate(pb, input_size_in_field_elements, "input_as_field_elements");
        this->pb.set_input_sizes(input_size_in_field_elements);
    }

    h1_var.reset(new digest_variable<FieldT>(pb, sha256_digest_len, "h1"));
    input_as_bits.insert(input_as_bits.end(), h1_var->bits.begin(), h1_var->bits.end());

    // Multipacking
    assert(input_as_bits.size() == input_size_in_bits);
    unpack_inputs.reset(new multipacking_gadget<FieldT>(this->pb, input_as_bits, input_as_field_elements, FieldT::capacity(), FMT(this->annotation_prefix, " unpack_inputs")));


/*
    pb_linear_combination_array<FieldT> IV = SHA256_default_IV(pb);

    r1_var.reset(new digest_variable<FieldT>(pb, sha256_digest_len, "r1"));
    h1_var.reset(new digest_variable<FieldT>(pb, sha256_digest_len, "h1"));
    h_r1_block.reset(new block_variable<FieldT>(pb, {r1_var->bits}, "h_r1_block"));
    h_r1.reset(new sha256_compression_function_gadget<FieldT>(pb,
                                                              IV,
                                                              h_r1_block->bits,
                                                              *h1_var, "h_r1"));
*/
}

template<typename FieldT>
void example_gadget<FieldT>::generate_r1cs_constraints()
{
    unpack_inputs->generate_r1cs_constraints(true);
    h1_var->generate_r1cs_constraints();
}

template<typename FieldT>
void example_gadget<FieldT>::generate_r1cs_witness(const bit_vector &h1
                                                  )
{
    h1_var->bits.fill_with_bits(this->pb, h1);

    unpack_inputs->generate_r1cs_witness_from_bits();
}

template<typename FieldT>
r1cs_primary_input<FieldT> example_input_map(const bit_vector &h1)
{
    assert(h1.size() == sha256_digest_len);
    bit_vector input_as_bits;
    input_as_bits.insert(input_as_bits.end(), h1.begin(), h1.end());
    std::vector<FieldT> input_as_field_elements = pack_bit_vector_into_field_element_vector<FieldT>(input_as_bits);
    return input_as_field_elements;
}