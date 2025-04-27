#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <flint/acb.h>
#include <flint/acb_mat.h>
#include "acb_vector.hpp"

void fill_matrix(acb_mat_t matrix, slong m, const acb_vector& zeros, slong precision);
void solve_all(const acb_vector& zeros, slong max_m, slong precision);

#endif