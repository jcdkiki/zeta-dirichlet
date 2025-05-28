#ifndef READER_HPP
#define READER_HPP

#include <flint_wrappers/vector.hpp>

void read_zeros(flint::Vector &zeros, const char *filepath, slong n_zeros, slong precision);

void print_zeros(const flint::Vector &zeros, slong n_zeros, slong precision);

#endif
