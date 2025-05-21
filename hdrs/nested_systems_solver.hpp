#ifndef NESTED_SYSTEMS_SOLVER_HPP
#define NESTED_SYSTEMS_SOLVER_HPP

#include <vector>
#include "utils.hpp"
#include "acb_vector.hpp"
#include "acb_matrix.hpp"

class NestedSystemsSolver
{
private:
    const std::vector<coefficient>& fixed_coefficients;
    const acb_vector& zeros;
    const slong max_system_size = zeros.get_size() + fixed_coefficients.size();
    const slong precision;
    std::vector<acb_vector> solutions;
    acb_matrix matrix;
    acb_matrix rhs;

private:
    void fill_matrix();
    void fill_rhs();

public:
    NestedSystemsSolver(
        std::vector<coefficient>& fixed_coefs, 
        acb_vector& zeta_zeros, 
        slong precision = BYTE_PRECISION);

    void solve_all_nested();

    acb_vector& get_coefs_vector(int idx);

};

#endif