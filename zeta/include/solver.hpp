#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <flint/acb.h>
#include <vector>
#include <acb_wrappers/matrix.hpp>
#include "utils.hpp"

void fill_rhs(acb_matrix& rhs, const std::vector<coefficient>& fixed_coefficients, slong size);

void fill_rhs(acb::Vector& rhs, const std::vector<coefficient>& fixed_coefficients, slong size);

void fill_matrix(acb_matrix& matrix, const std::vector<coefficient>& fixed_coefficients, const acb::Vector& zeros, slong precision);

void solve(acb_matrix& res, const acb::Vector& zeros, const std::vector<coefficient>& fixed_coefficients, slong precision);

void solve_all(acb_matrix& res, const acb::Vector& zeros, const std::vector<coefficient>& fixed_coefficients, slong precision);

#endif
