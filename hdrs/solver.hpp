#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <flint/acb.h>
#include <map>
#include <vector>
#include "acb_matrix.hpp"
#include "acb_vector.hpp"
#include "utils.hpp"

void fill_matrix(acb_matrix& matrix, std::vector<coefficient>& fixated_coefficients, const acb_vector& zeros, slong precision);

void solve(acb_matrix& res, const acb_vector& zeros, std::vector<coefficient>& fixated_coefficients, slong precision);

void solve_all(const acb_vector& zeros, slong max_m, slong precision, const char* filename = nullptr);

void compute_series(acb_t result, const acb_t s, int m, const std::map<int, acb_vector>& solutions, slong precision);

#endif