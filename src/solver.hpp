#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <flint/acb.h>
#include <flint/acb_mat.h>
#include <map>
#include "acb_vector.hpp"

void fill_matrix(acb_mat_t matrix, slong m, const acb_vector& zeros, slong precision);
void solve_all(const acb_vector& zeros, slong max_m, slong precision, const char* filename = nullptr);
void compute_series(acb_t result, const acb_t s, int m, const std::map<int, acb_vector>& solutions, slong precision);

#endif