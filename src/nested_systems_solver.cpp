#include "nested_systems_solver.hpp"
#include "common.hpp"
#include <flint/arb.h>
#include <flint/fmpq.h>
#include <limits>
#include <future>

// TODO: fix meeeee
static constexpr int BYTE_PRECISION = 2048;

NestedSystemsSolver::NestedSystemsSolver(std::vector<coefficient>& fixed_coefs, acb::Vector& zeta_zeros, slong precision)
    : zeros(zeta_zeros), fixed_coefficients(fixed_coefs), precision(precision), matrix(max_system_size, max_system_size), rhs(max_system_size, 1) {}

acb::Vector& NestedSystemsSolver::get_coefs_vector(int idx) 
{
    return solutions[idx];
}

void NestedSystemsSolver::fill_matrix()
{
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    slong n_coefs = fixed_coefficients.size();
    slong n_zeros = zeros.size();

    coefficient current_coef = {-1, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()};

    if (n_coefs > 0) 
    {
        current_coef = fixed_coefficients[0];
    }

    slong coeff_ptr = 0;
    slong zero_ptr  = 0;

    slong row = 0, col = 0;

    while (row < max_system_size)
    {   
        if (row == current_coef.idx) {
            acb_set_d_d(acb_mat_entry(*matrix.get_mat(), row, row), 1.0, 0.0);

            coeff_ptr++;
            if (coeff_ptr < n_coefs) current_coef = fixed_coefficients[coeff_ptr];
            else current_coef.idx = max_system_size + 1;
            row++;
            continue;
        }

        for (slong col = 0; col < max_system_size; ++col) 
        {
            acb_set_d_d(base, col + 1, 0.0);       // base = (col+1) + 0i
            acb_set(exp, zeros[zero_ptr]);         // exp = zeros[zero_ptr]
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision);    // res = base^-exp
            acb_set(acb_mat_entry(*matrix.get_mat(), row, col), res);
        }

        zero_ptr++;
        row++;
    }

    acb_clear(base);
    acb_clear(exp);
    acb_clear(res);
}

void NestedSystemsSolver::fill_rhs()
{
    for (const auto& coef: fixed_coefficients)
    {
        acb_set_d_d(acb_mat_entry(*rhs.get_mat(), coef.idx, 0), coef.re_value, coef.im_value);
    }
}

void NestedSystemsSolver::initialize_identity_matrix(acb_matrix& P) 
{
    for (slong i = 0; i < max_system_size; ++i) {
        for (slong j = 0; j < max_system_size; ++j) {
            if (i == j) {
                acb_one(acb_mat_entry(*P.get_mat(), i, j));
            } else {
                acb_zero(acb_mat_entry(*P.get_mat(), i, j));
            }
        }
    }
}

void NestedSystemsSolver::initialize_base_case(acb_matrix& L, acb_matrix& U, acb::Vector& y, acb::Vector& x) 
{
    acb_set_d_d(acb_mat_entry(*L.get_mat(), 0, 0), 1.0, 0.0);
    acb_set(acb_mat_entry(*U.get_mat(), 0, 0), acb_mat_entry(*matrix.get_mat(), 0, 0));
    acb_div(y[0], acb_mat_entry(*rhs.get_mat(), 0, 0), acb_mat_entry(*U.get_mat(), 0, 0), BYTE_PRECISION);
    acb_set(x[0], y[0]);
    solutions.push_back(x);
}

slong NestedSystemsSolver::find_pivot_index(slong k, const acb_matrix& matrix) 
{
    arb_t pivot, current;
    arb_init(pivot);
    arb_init(current);
    acb_abs(pivot, acb_mat_entry(*matrix.get_mat(), k, k), BYTE_PRECISION);
    slong pivot_idx = k;

    for (slong i = 0; i < k + 1; ++i) 
    {
        acb_abs(current, acb_mat_entry(*matrix.get_mat(), i, k), BYTE_PRECISION);
        if (arb_gt(current, pivot)) 
        {
            arb_set(pivot, current);
            pivot_idx = i;
        }
    }

    arb_clear(pivot);
    arb_clear(current);
    return pivot_idx;
}

void NestedSystemsSolver::swap_rows(acb_matrix& matrix, acb_matrix& rhs, acb_matrix& L, acb_matrix& P, slong k, slong pivot_idx) 
{
    acb_t swapping_buffer;
    acb_init(swapping_buffer);

    if (pivot_idx != k) 
    {
        for (slong j = 0; j < max_system_size; ++j) 
        {
            // Swap matrix rows
            acb_set(swapping_buffer, acb_mat_entry(*matrix.get_mat(), k, j));
            acb_set(acb_mat_entry(*matrix.get_mat(), k, j), acb_mat_entry(*matrix.get_mat(), pivot_idx, j));
            acb_set(acb_mat_entry(*matrix.get_mat(), pivot_idx, j), swapping_buffer);

            // Swap L rows
            acb_set(swapping_buffer, acb_mat_entry(*L.get_mat(), k, j));
            acb_set(acb_mat_entry(*L.get_mat(), k, j), acb_mat_entry(*L.get_mat(), pivot_idx, j));
            acb_set(acb_mat_entry(*L.get_mat(), pivot_idx, j), swapping_buffer);

            // Swap P rows
            acb_set(swapping_buffer, acb_mat_entry(*P.get_mat(), k, j));
            acb_set(acb_mat_entry(*P.get_mat(), k, j), acb_mat_entry(*P.get_mat(), pivot_idx, j));
            acb_set(acb_mat_entry(*P.get_mat(), pivot_idx, j), swapping_buffer);
        }

        // Swap rhs rows
        acb_set(swapping_buffer, acb_mat_entry(*rhs.get_mat(), k, 0));
        acb_set(acb_mat_entry(*rhs.get_mat(), k, 0), acb_mat_entry(*rhs.get_mat(), pivot_idx, 0));
        acb_set(acb_mat_entry(*rhs.get_mat(), pivot_idx, 0), swapping_buffer);
    }

    acb_clear(swapping_buffer);
}

void NestedSystemsSolver::compute_u_vector(acb::Vector& u, slong k)
{
    for (slong i = 0; i < k; ++i) 
    {
        acb_set(u[i], acb_mat_entry(*matrix.get_mat(), i, k));
    }
}

void NestedSystemsSolver::compute_w_vector(acb::Vector& w, const acb::Vector& u, const acb_matrix& L, slong k) {
    acb_t temp, temp_sum;
    acb_init(temp);
    acb_init(temp_sum);

    for (slong i = 0; i < k; ++i) 
    {
        acb_zero(temp_sum);
        for (slong j = 0; j < i; ++j) 
        {
            acb_mul(temp, acb_mat_entry(*L.get_mat(), i, j), w[j], BYTE_PRECISION);
            acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        acb_sub(w[i], u[i], temp_sum, BYTE_PRECISION);
    }

    acb_clear(temp);
    acb_clear(temp_sum);
}

void NestedSystemsSolver::compute_v_vector(acb::Vector& v, slong k) 
{
    for (slong i = 0; i < k; ++i) 
    {
        acb_set(v[i], acb_mat_entry(*matrix.get_mat(), k, i));
    }
}

void NestedSystemsSolver::compute_l_vector(acb::Vector& l, const acb::Vector& v, const acb_matrix& U, slong k) {
    acb_t temp, temp_sum;
    acb_init(temp);
    acb_init(temp_sum);

    for (slong i = k - 1; i >= 0; --i) 
    {
        acb_zero(temp_sum);
        for (slong j = i + 1; j < k; ++j) 
        {
            acb_mul(temp, acb_mat_entry(*U.get_mat(), i, j), l[j], BYTE_PRECISION);
            acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        acb_sub(l[i], v[i], temp_sum, BYTE_PRECISION);
        acb_div(l[i], l[i], acb_mat_entry(*U.get_mat(), i, i), BYTE_PRECISION);
    }

    acb_clear(temp);
    acb_clear(temp_sum);
}

void NestedSystemsSolver::compute_s(acb_t& s, const acb::Vector& l, const acb::Vector& w, slong k) 
{
    acb_t temp_dot_prod;
    acb_init(temp_dot_prod);

    dot_product(temp_dot_prod, l, w);
    acb_sub(s, acb_mat_entry(*matrix.get_mat(), k, k), temp_dot_prod, BYTE_PRECISION);

    acb_clear(temp_dot_prod);
}

void NestedSystemsSolver::update_LU_matrices(acb_matrix& L, acb_matrix& U, const acb::Vector& l, const acb::Vector& w, const acb_t& s, slong k) 
{
    for (slong i = 0; i < k; ++i) {
        acb_set(acb_mat_entry(*L.get_mat(), k, i), l[i]);
        acb_set(acb_mat_entry(*U.get_mat(), i, k), w[i]);
    }
    acb_set_d_d(acb_mat_entry(*L.get_mat(), k, k), 1.0, 0.0);
    acb_set(acb_mat_entry(*U.get_mat(), k, k), s);
}

void NestedSystemsSolver::solve_forward_substitution(acb::Vector& y, const acb_matrix& L, const acb_matrix& rhs, slong size) 
{
    acb_t temp, temp_sum;
    acb_init(temp);
    acb_init(temp_sum);

    for (slong i = 0; i < size; ++i) {
        acb_zero(temp_sum);
        for (slong j = 0; j < i; ++j) {
            acb_mul(temp, acb_mat_entry(*L.get_mat(), i, j), y[j], BYTE_PRECISION);
            acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        acb_sub(y[i], acb_mat_entry(*rhs.get_mat(), i, 0), temp_sum, BYTE_PRECISION);
    }

    acb_clear(temp);
    acb_clear(temp_sum);
}

void NestedSystemsSolver::solve_backward_substitution(acb::Vector& x, const acb::Vector& y, const acb_matrix& U, slong size) 
{
    acb_t temp, temp_sum;
    acb_init(temp);
    acb_init(temp_sum);

    for (slong i = size - 1; i >= 0; --i) {
        acb_zero(temp_sum);
        for (slong j = i + 1; j < size; ++j) {
            acb_mul(temp, acb_mat_entry(*U.get_mat(), i, j), x[j], BYTE_PRECISION);
            acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        acb_sub(x[i], y[i], temp_sum, BYTE_PRECISION);
        acb_div(x[i], x[i], acb_mat_entry(*U.get_mat(), i, i), BYTE_PRECISION);
    }

    acb_clear(temp);
    acb_clear(temp_sum);
}

void NestedSystemsSolver::lu_solve_all() 
{
    fill_matrix();
    fill_rhs();

    acb_matrix L(max_system_size, max_system_size);
    acb_matrix U(max_system_size, max_system_size);
    acb_matrix P(max_system_size, max_system_size);

    acb::Vector x(1), y(1);
    initialize_identity_matrix(P);
    initialize_base_case(L, U, x, y);

    for (slong size = 2; size <= max_system_size; ++size) 
    {
        slong k = size - 1;

        // Pivoting
        slong pivot_idx = find_pivot_index(k, matrix);
        swap_rows(matrix, rhs, L, P, k, pivot_idx);

        // LU decomposition
        acb::Vector u(k), w(k), v(k), l(k);
        compute_u_vector(u, k);
        compute_w_vector(w, u, L, k);
        compute_v_vector(v, k);
        compute_l_vector(l, v, U, k);

        acb_t s;
        acb_init(s);
        compute_s(s, l, w, k);
        update_LU_matrices(L, U, l, w, s, k);

        // Solve system
        acb::Vector y(size), x(size);
        solve_forward_substitution(y, L, rhs, size);
        solve_backward_substitution(x, y, U, size);

        solutions.push_back(x);
        acb_clear(s);
    }
}

void NestedSystemsSolver::modified_gram_schmidt(acb_matrix &Q, acb_matrix &R, const acb_matrix &A) 
{
    slong n = A.row_count();
    
    if (A.col_count() != n) {
        throw ZetaException("Matrix must be square");
    }

    for (slong j = 0; j < n; j++) 
    {
        acb::Vector v(n);
        
        for (slong i = 0; i < n; i++) 
        {
            acb_set(v[i], acb_mat_entry(*A.get_mat(), i, j));
        }

        for (slong i = 0; i < j; i++) 
        {
            acb_t dot;
            acb_init(dot);

            acb::Vector qi(n);
            for (slong k = 0; k < n; k++) 
            {
                acb_set(qi[k], acb_mat_entry(*Q.get_mat(), k, i));
            }

            dot_product(dot, qi, v);
            acb_set(acb_mat_entry(*R.get_mat(), i, j), dot);

            acb::Vector scaled_qi = scalar_multiply(qi, dot);
            v = subtract_vectors(v, scaled_qi);

            acb_clear(dot);
        }

        acb_t norm;
        acb_init(norm);
        vector_norm(norm, v);
        acb_set(acb_mat_entry(*R.get_mat(), j, j), norm);

        if (!acb_is_zero(norm)) 
        {
            for (slong i = 0; i < n; i++) 
            {
                acb_div(acb_mat_entry(*Q.get_mat(), i, j), v[i], norm, BYTE_PRECISION);
            }
        }

        acb_clear(norm);
    }
}

acb::Vector NestedSystemsSolver::solve_upper_triangular(const acb_matrix &R, const acb::Vector &b) 
{
    slong n = R.row_count();

    if (R.col_count() != n || b.size() != n) {
        throw ZetaException("Matrix must be square and vector must match the matrix size");
    }

    acb::Vector x(n);

    for (slong i = n - 1; i >= 0; i--) 
    {
        acb_t diag;
        acb_init(diag);
        acb_set(diag, acb_mat_entry(*R.get_mat(), i, i));

        if (acb_is_zero(diag)) {
            throw ZetaException("Matrix is singular or poorly conditioned");
        }

        acb_set(x[i], b[i]);

        for (slong j = i + 1; j < n; j++)
        {
            acb_t temp;
            acb_init(temp);
            acb_mul(temp, acb_mat_entry(*R.get_mat(), i, j), x[j], BYTE_PRECISION);
            acb_sub(x[i], x[i], temp, BYTE_PRECISION);
            acb_clear(temp);
        }

        acb_div(x[i], x[i], diag, BYTE_PRECISION);
        acb_clear(diag);
    }

    return x;
}

acb::Vector NestedSystemsSolver::qr_solve_system(const acb_matrix &A_k, const acb::Vector &b_k) 
{
    slong k = A_k.row_count();

    if (A_k.col_count() != k || b_k.size() != k) 
    {
        throw ZetaException("Matrix must be square and vector must match the matrix size");
    }

    acb_matrix Q(k, k);
    acb_matrix R(k, k);
    modified_gram_schmidt(Q, R, A_k);

    acb::Vector Q_T_b(k);
    for (slong i = 0; i < k; i++) 
    {
        acb::Vector q_i(k);
        for (slong j = 0; j < k; j++) 
        {
            acb_set(q_i[j], acb_mat_entry(*Q.get_mat(), j, i));
        }
        dot_product(Q_T_b[i], q_i, b_k);
    }

    return solve_upper_triangular(R, Q_T_b);
}

void prepare_subsystem(acb_matrix &A_k, acb::Vector &b_k, const acb_matrix &A, const acb::Vector &b, slong k) 
{
    if (A.row_count() < k || A.col_count() < k || b.size() < k) {
        throw ZetaException("Requested submatrix size exceeds original dimensions");
    }

    A_k = acb_matrix(k, k);
    b_k = acb::Vector(k);

    for (slong i = 0; i < k; i++) {
        acb_set(b_k[i], b[i]);
        for (slong j = 0; j < k; j++) {
            acb_set(acb_mat_entry(*A_k.get_mat(), i, j), acb_mat_entry(*A.get_mat(), i, j));
        }
    }
}

void NestedSystemsSolver::qr_solve_all() 
{   
    fill_matrix();
    fill_rhs();
    std::vector<std::future<acb::Vector>> futures;

    acb::Vector temp_rhs(rhs(0), max_system_size);

    for (slong k = 1; k <= max_system_size; k++) 
    {
        futures.emplace_back(std::async(std::launch::async, [&, k]() 
        {
            acb_matrix A_k(k, k);
            acb::Vector b_k(k);
            prepare_subsystem(A_k, b_k, matrix, temp_rhs, k);
            return qr_solve_system(A_k, b_k);
        }));
    }

    for (auto &f : futures) 
    {
        solutions.push_back(f.get());
    }
}

void NestedSystemsSolver::slow_solve_all()
{
    fill_matrix();
    fill_rhs();

    std::vector<std::future<acb::Vector>> futures;
    acb::Vector temp_rhs(rhs(0), max_system_size);

    for (slong k = 1; k <= max_system_size; k++) 
    {
        futures.emplace_back(std::async(std::launch::async, [&, k]() 
        {
            acb_matrix x(k, 1);
            acb_matrix A_k(k, k);
            acb::Vector b_k(k);
            prepare_subsystem(A_k, b_k, matrix, temp_rhs, k);
            
            acb_matrix b_k_temp(b_k);
            acb_mat_solve(*x.get_mat(), *A_k.get_mat(), *b_k_temp.get_mat(), precision);

            return acb::Vector(x(0), k);
        }));
    }

    for (auto &f : futures) 
    {
        solutions.push_back(f.get());
    }
}
