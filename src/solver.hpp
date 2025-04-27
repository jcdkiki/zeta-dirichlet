#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <flint/acb.h>
#include <flint/acb_mat.h>
#include "acb_vector.hpp"

void make_full_matrix(acb_mat_t& matrix, slong n, acb_vector& zeros, slong precision);
void solve_all(acb_mat_t& A, slong n, slong precision);

#endif