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

private:
    // lu funcs
    void initialize_identity_matrix(acb_matrix& P);
    
    void initialize_base_case(acb_matrix& L, acb_matrix& U, acb_vector& y, acb_vector& x);

    slong find_pivot_index(slong k, const acb_matrix& matrix);

    void swap_rows(acb_matrix& matrix, acb_matrix& rhs, acb_matrix& L, acb_matrix& P, slong k, slong pivot_idx);

    void compute_u_vector(acb_vector& u, slong k);

    void compute_w_vector(acb_vector& w, const acb_vector& u, const acb_matrix& L, slong k);

    void compute_v_vector(acb_vector& v, slong k);

    void compute_l_vector(acb_vector& l, const acb_vector& v, const acb_matrix& U, slong k);

    void compute_s(acb_t& s, const acb_vector& l, const acb_vector& w, slong k);

    void update_LU_matrices(acb_matrix& L, acb_matrix& U, const acb_vector& l, const acb_vector& w, const acb_t& s, slong k);

    void solve_forward_substitution(acb_vector& y, const acb_matrix& L, const acb_matrix& rhs, slong size);

    void solve_backward_substitution(acb_vector& x, const acb_vector& y, const acb_matrix& U, slong size);

private:
    // qr funcs
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