#ifndef NESTED_SYSTEMS_SOLVER_HPP
#define NESTED_SYSTEMS_SOLVER_HPP

#include <vector>
#include "utils.hpp"
#include <acb_wrappers/matrix.hpp>

void prepare_subsystem(acb_matrix &A_k, acb::Vector &b_k, const acb_matrix &A, const acb::Vector &b, slong k);

class NestedSystemsSolver {
private:
    const std::vector<coefficient>& fixed_coefficients;
    const acb::Vector& zeros;
    const slong max_system_size = zeros.size() + fixed_coefficients.size();
    std::vector<acb::Vector> solutions;
    acb_matrix matrix;
    acb_matrix rhs;
    slong precision;

    void fill_matrix();
    void fill_rhs();

    // lu funcs
    void initialize_identity_matrix(acb_matrix& P);
    void initialize_base_case(acb_matrix& L, acb_matrix& U, acb::Vector& y, acb::Vector& x);

    slong find_pivot_index(slong k, const acb_matrix& matrix);

    void swap_rows(acb_matrix& matrix, acb_matrix& rhs, acb_matrix& L, acb_matrix& P, slong k, slong pivot_idx);

    void compute_u_vector(acb::Vector& u, slong k);
    void compute_w_vector(acb::Vector& w, const acb::Vector& u, const acb_matrix& L, slong k);
    void compute_v_vector(acb::Vector& v, slong k);
    void compute_l_vector(acb::Vector& l, const acb::Vector& v, const acb_matrix& U, slong k);
    void compute_s(acb_t& s, const acb::Vector& l, const acb::Vector& w, slong k);

    void update_LU_matrices(acb_matrix& L, acb_matrix& U, const acb::Vector& l, const acb::Vector& w, const acb_t& s, slong k);

    void solve_forward_substitution(acb::Vector& y, const acb_matrix& L, const acb_matrix& rhs, slong size);
    void solve_backward_substitution(acb::Vector& x, const acb::Vector& y, const acb_matrix& U, slong size);

    // qr funcs
    void modified_gram_schmidt(acb_matrix &Q, acb_matrix &R, const acb_matrix &A);

    acb::Vector solve_upper_triangular(const acb_matrix &R, const acb::Vector &b);

    acb::Vector qr_solve_system(const acb_matrix &A_k, const acb::Vector &b_k);

public:
    NestedSystemsSolver(
        std::vector<coefficient>& fixed_coefs, 
        acb::Vector& zeta_zeros, 
        slong precision);

    void lu_solve_all();
    void qr_solve_all();
    void slow_solve_all();

    acb::Vector& get_coefs_vector(int idx);
};

#endif
