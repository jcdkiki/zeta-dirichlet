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
    const slong                     max_system_size = 50;
    std::vector<flint::Vector>      solutions;
    flint::Matrix                   matrix;
    flint::Matrix                   rhs;
    slong                           precision;

    void fill_matrix();
    void fill_matrix_nofix();
    void fill_rhs();

    // lu funcs
    void compute_lu_decomposition(flint::Matrix &L, flint::Vector &diagonal);

    void foraward_substitution(flint::Matrix &L, flint::Vector &diagonal);

    // qr funcs
    void modified_gram_schmidt(flint::Matrix &Q, flint::Matrix &R, const flint::Matrix &A);

    flint::Vector solve_upper_triangular(const flint::Matrix &R, const flint::Vector &b);

    flint::Vector qr_solve_system(const flint::Matrix &A_k, const flint::Vector &b_k);

  public:
    NestedSystemsSolver(std::vector<coefficient> &fixed_coefs, flint::Vector &zeta_zeros,
                        slong precision);
    
    void optimized_lu_solve_all();
    void qr_solve_all();
    void slow_solve_all();

    flint::Vector &get_coefs_vector(int idx);
};

#endif
