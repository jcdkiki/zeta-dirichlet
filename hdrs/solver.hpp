#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <flint/acb.h>
#include <map>
#include <vector>
#include "acb_matrix.hpp"
#include "acb_vector.hpp"
#include "utils.hpp"

void fill_rhs(acb_matrix& rhs, const std::vector<coefficient>& fixed_coefficients, slong size);

void fill_rhs(acb_vector& rhs, const std::vector<coefficient>& fixed_coefficients, slong size);

void fill_matrix(acb_matrix& matrix, const std::vector<coefficient>& fixed_coefficients, const acb_vector& zeros, slong precision);

void solve(acb_matrix& res, const acb_vector& zeros, const std::vector<coefficient>& fixed_coefficients, slong precision);

void solve_all(acb_matrix& res, const acb_vector& zeros, const std::vector<coefficient>& fixed_coefficients, slong precision);

#endif