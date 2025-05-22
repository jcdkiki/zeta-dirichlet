#ifndef NESTED_SYSTEMS_SOLVER_HPP
#define NESTED_SYSTEMS_SOLVER_HPP

#include <vector>
#include "utils.hpp"
#include "acb_vector.hpp"
#include "acb_matrix.hpp"

void prepare_subsystem(acb_matrix &A_k, acb_vector &b_k, const acb_matrix &A, const acb_vector &b, slong k);

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

    void modified_gram_schmidt(acb_matrix &Q, acb_matrix &R, const acb_matrix &A);

    acb_vector solve_upper_triangular(const acb_matrix &R, const acb_vector &b);

    acb_vector qr_solve_system(const acb_matrix &A_k, const acb_vector &b_k);

public:
    NestedSystemsSolver(
        std::vector<coefficient>& fixed_coefs, 
        acb_vector& zeta_zeros, 
        slong precision = BYTE_PRECISION);

    void lu_solve_all();

    void qr_solve_all();

    acb_vector& get_coefs_vector(int idx);

};

#endif