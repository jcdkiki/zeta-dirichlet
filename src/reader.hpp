#ifndef READER_HPP
#define READER_HPP

#include "acb_vector.hpp"
#include <flint/acb.h>
#include <flint/acb_mat.h>
#include <flint/arb.h>
#include <fstream>
#include <iostream>
#include <string>

const std::string path = "/mnt/1DE0B2F616840AF3/downloads/zeros.val";

void read_zeros(acb_vector &zeros, const std::string &filepath, slong n_zeros,
                slong precision);

void print_zeros(const acb_vector &zeros, slong n_zeros, slong precision);

#endif
