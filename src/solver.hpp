#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "acb_vector.hpp"
#include <flint/acb.h>
#include <flint/acb_calc.h>
#include <flint/acb_mat.h>
#include <flint/arb.h>

void make_matrix(acb_mat_t &matrix, slong matrix_size, acb_vector &zeros,
                 slong precision);

void solve(acb_mat_t &a, slong n, slong precision);

#endif
