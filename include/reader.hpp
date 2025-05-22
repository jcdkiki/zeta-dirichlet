#ifndef READER_HPP
#define READER_HPP

#include "acb_vector.hpp"
#include <flint/acb.h>
#include <flint/acb_mat.h>
#include <flint/arb.h>

void read_zeros(acb::Vector &zeros, const char* filepath, slong n_zeros, slong precision);

void print_zeros(const acb::Vector &zeros, slong n_zeros, slong precision);

#endif
