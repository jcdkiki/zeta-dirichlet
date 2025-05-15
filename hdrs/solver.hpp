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

void solve_all(acb_matrix& res, const acb_vector& zeros, std::vector<coefficient>& fixated_coefficients, slong precision);

#endif