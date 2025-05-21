#ifndef NESTED_SYSTEMS_SOLVER_HPP
#define NESTED_SYSTEMS_SOLVER_HPP

#include <vector>
#include "utils.hpp"
#include "acb_vector.hpp"
#include "acb_matrix.hpp"

class NestedSystemsSolver
{
private:
    std::vector<acb_vector> solutions;
    const std::vector<coefficient>& fixed_coefficients;
    const acb_vector& zeros;
    const slong max_system_size = zeros.get_size() + fixed_coefficients.size();
    const slong precision;

private:
    int find_current_fix_coeffs_num(slong current_n_zeros);

    void fill_initial_matrix(acb_matrix& matrix, slong current_system_size, slong current_n_zeros);

    void update_matrix(acb_matrix& matrix, slong prev_n_zeros, slong current_n_zeros, slong current_system_size);

    void fill_initial_rhs(acb_vector& rhs, slong current_system_size, slong current_n_zeros);

    void update_rhs(acb_vector& rhs, slong prev_n_zeros, slong current_n_zeros, slong current_system_size);

    void compute_initial_lu(acb_matrix& matrix, acb_matrix& L, acb_matrix& U, slong system_size);

    void update_lu(acb_matrix& matrix, acb_matrix& L, acb_matrix& U, slong prev_size, slong new_size);

    void solve_system(acb_matrix& L, acb_matrix& U, acb_vector& b, acb_vector& x, slong system_size);

public:
    NestedSystemsSolver(
        std::vector<coefficient>& fixed_coefs, 
        acb_vector& zeta_zeros, 
        slong precision = BYTE_PRECISION);

    void solve_all_nested();

    acb_vector& get_coefs_vector(int idx);
    
};

#endif