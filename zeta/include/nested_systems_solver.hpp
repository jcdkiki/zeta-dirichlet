#ifndef NESTED_SYSTEMS_SOLVER_HPP
#define NESTED_SYSTEMS_SOLVER_HPP

#include "utils.hpp"
#include <flint_wrappers/matrix.hpp>
#include <vector>

void prepare_subsystem(flint::Matrix &A_k, flint::Vector &b_k, const flint::Matrix &A,
                       const flint::Vector &b, slong k);

class NestedSystemsSolver {
  private:
    const std::vector<coefficient> &fixed_coefficients;
    const flint::Vector            &zeros;
    const slong                     max_system_size = zeros.size() + fixed_coefficients.size();
    std::vector<flint::Vector>      solutions;
    flint::Matrix                   matrix;
    flint::Matrix                   rhs;
    slong                           precision;

    void fill_matrix();
    void fill_rhs();

    // lu funcs
    void initialize_identity_matrix(flint::Matrix &P);
    void initialize_base_case(flint::Matrix &L, flint::Matrix &U, flint::Vector &y,
                              flint::Vector &x);

    slong find_pivot_index(slong k, const flint::Matrix &matrix);

    void swap_rows(flint::Matrix &matrix, flint::Matrix &rhs, flint::Matrix &L, flint::Matrix &P,
                   slong k, slong pivot_idx);

    void compute_u_vector(flint::Vector &u, slong k);
    void compute_w_vector(flint::Vector &w, const flint::Vector &u, const flint::Matrix &L,
                          slong k);
    void compute_v_vector(flint::Vector &v, slong k);
    void compute_l_vector(flint::Vector &l, const flint::Vector &v, const flint::Matrix &U,
                          slong k);
    void compute_s(flint::Complex &s, const flint::Vector &l, const flint::Vector &w, slong k);

    void update_LU_matrices(flint::Matrix &L, flint::Matrix &U, const flint::Vector &l,
                            const flint::Vector &w, const flint::Complex &s, slong k);

    void solve_forward_substitution(flint::Vector &y, const flint::Matrix &L,
                                    const flint::Matrix &rhs, slong size);
    void solve_backward_substitution(flint::Vector &x, const flint::Vector &y,
                                     const flint::Matrix &U, slong size);

    // qr funcs
    void modified_gram_schmidt(flint::Matrix &Q, flint::Matrix &R, const flint::Matrix &A);

    flint::Vector solve_upper_triangular(const flint::Matrix &R, const flint::Vector &b);

    flint::Vector qr_solve_system(const flint::Matrix &A_k, const flint::Vector &b_k);

  public:
    NestedSystemsSolver(std::vector<coefficient> &fixed_coefs, flint::Vector &zeta_zeros,
                        slong precision);

    void lu_solve_all();
    void qr_solve_all();
    void slow_solve_all();

    flint::Vector &get_coefs_vector(int idx);
};

#endif
