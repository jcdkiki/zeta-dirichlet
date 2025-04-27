#ifndef READER_HPP
#define READER_HPP

#include <flint/acb.h>
#include <flint/acb_mat.h>
#include <flint/arb.h>
#include <string>
#include <fstream>
#include <iostream>
#include "acb_vector.hpp" 

void read_zeros(acb_vector& zeros, const std::string& filepath, slong n_zeros, slong precision);

void print_zeros(const acb_vector& zeros, slong n_zeros, slong precision);

#endif