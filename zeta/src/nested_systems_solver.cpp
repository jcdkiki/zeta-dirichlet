#include "nested_systems_solver.hpp"
#include "common.hpp"
#include "flint_wrappers/matrix.hpp"
#include <future>
#include <limits>

// TODO: fix meeeee
static constexpr int BYTE_PRECISION = 2048;

NestedSystemsSolver::NestedSystemsSolver(std::vector<coefficient> &fixed_coefs,
                                         flint::Vector &zeta_zeros, slong precision)
    : zeros(zeta_zeros), fixed_coefficients(fixed_coefs), precision(precision),
      matrix(max_system_size, max_system_size), rhs(max_system_size, 1)
{
}

flint::Vector &NestedSystemsSolver::get_coefs_vector(int idx)
{
    return solutions[idx];
}

void NestedSystemsSolver::fill_matrix()
{
    flint::Complex base, exp, res;
    
    slong n_coefs = fixed_coefficients.size();
    slong n_zeros = zeros.size();

    coefficient current_coef = {-1, std::numeric_limits<double>::infinity(),
                                std::numeric_limits<double>::infinity()};

    if (n_coefs > 0) {
        current_coef = fixed_coefficients[0];
    }

    slong coeff_ptr = 0;
    slong zero_ptr = 0;

    slong row = 0, col = 0;

    while (row < max_system_size) {
        if (row == current_coef.idx) {
            matrix.at(row, row).set(1.0, 0.0);
            
            coeff_ptr++;
            if (coeff_ptr < n_coefs)
                current_coef = fixed_coefficients[coeff_ptr];
            else
                current_coef.idx = max_system_size + 1;
            row++;
            continue;
        }

        for (slong col = 0; col < max_system_size; ++col) {
            base.set((double)(col + 1), 0.0);       // base = (col+1) + 0i
            exp.set(zeros[zero_ptr]);               // exp = zeros[zero_ptr]
            exp.neg();                              // exp = -exp
            flint::pow(res, base, exp, precision);  // res = base^-exp
            flint::pow(res, base, exp, precision); 
            
            matrix.at(row, col).set(res);
        }

        zero_ptr++;
        row++;
    }
}

void NestedSystemsSolver::fill_rhs()
{
    for (const auto &coeff : fixed_coefficients) {
        rhs.at(coeff.idx, 0).set(coeff.real, coeff.imag);
    }
}

void NestedSystemsSolver::initialize_identity_matrix(flint::Matrix &P)
{
    P.zero();

    for (slong i = 0; i < max_system_size; ++i) {
        P.at(i, i).set(1);
    }
}

void NestedSystemsSolver::initialize_base_case(flint::Matrix &L, flint::Matrix &U, flint::Vector &y, flint::Vector &x)
{
    L.at(0, 0).set(1.0, 0.0);
    U.at(0, 0).set(matrix.at(0, 0));
    flint::div(y[0], rhs.at(0, 0), U.at(0, 0), BYTE_PRECISION);
    x[0].set(y[0]);
    solutions.push_back(x);
}

slong NestedSystemsSolver::find_pivot_index(slong k, const flint::Matrix &matrix)
{
    flint::Real pivot, current;
    
    flint::abs(pivot, matrix.at(k, k), BYTE_PRECISION);
    slong pivot_idx = k;

    for (slong i = 0; i < k + 1; ++i) {
        flint::abs(current, matrix.at(i, k), BYTE_PRECISION);
        if (current > pivot) {
            pivot.set(current);
            pivot_idx = i;
        }
    }

    return pivot_idx;
}

void NestedSystemsSolver::swap_rows(flint::Matrix &matrix, flint::Matrix &rhs, flint::Matrix &L,
                                    flint::Matrix &P, slong k, slong pivot_idx)
{
    flint::Complex temp;
    
    if (pivot_idx != k) {
        for (slong j = 0; j < max_system_size; ++j) {
            // Swap matrix rows
            temp.set(matrix.at(k, j));
            matrix.at(k, j).set(matrix.at(pivot_idx, j));
            matrix.at(pivot_idx, j).set(temp);

            // Swap L rows
            temp.set(L.at(k, j));
            L.at(k, j).set(L.at(pivot_idx, j));
            L.at(pivot_idx, j).set(temp);
            
            // Swap P rows
            temp.set(P.at(k, j));
            P.at(k, j).set(P.at(pivot_idx, j));
            P.at(pivot_idx, j).set(temp);
        }

        // Swap rhs rows
        temp.set(rhs.at(k, 0));
        rhs.at(k, 0).set(rhs.at(pivot_idx, 0));
        rhs.at(pivot_idx, 0).set(temp);
    }
}

void NestedSystemsSolver::compute_u_vector(flint::Vector &u, slong k)
{
    for (slong i = 0; i < k; ++i) {
        u[i].set(matrix.at(i, k));
    }
}

void NestedSystemsSolver::compute_w_vector(flint::Vector &w, const flint::Vector &u,
                                           const flint::Matrix &L, slong k)
{
    flint::Complex temp, temp_sum;

    for (slong i = 0; i < k; ++i) {
        temp_sum.zero();
        for (slong j = 0; j < i; ++j) {
            flint::mul(temp, L.at(i, j), w[j], BYTE_PRECISION);
            flint::add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        flint::sub(w[i], u[i], temp_sum, BYTE_PRECISION);
    }
}

void NestedSystemsSolver::compute_v_vector(flint::Vector &v, slong k)
{
    for (slong i = 0; i < k; ++i) {
        matrix.at(k, i).set(v[i]);
    }
}

void NestedSystemsSolver::compute_l_vector(flint::Vector &l, const flint::Vector &v,
                                           const flint::Matrix &U, slong k)
{
    flint::Complex temp, temp_sum;

    for (slong i = k - 1; i >= 0; --i) {
        temp_sum.zero();
        for (slong j = i + 1; j < k; ++j) {
            flint::mul(temp, U.at(i, j), l[j], BYTE_PRECISION);
            flint::add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        flint::sub(l[i], v[i], temp_sum, BYTE_PRECISION);
        flint::div(l[i], l[i], U.at(i, i), BYTE_PRECISION);
    }
}

void NestedSystemsSolver::compute_s(flint::Complex &s, const flint::Vector &l, const flint::Vector &w, slong k)
{
    flint::Complex temp_dot;
    flint::dot(temp_dot, l, w, BYTE_PRECISION);
    flint::sub(s, matrix.at(k, k), temp_dot, BYTE_PRECISION);
}

void NestedSystemsSolver::update_LU_matrices(flint::Matrix &L, flint::Matrix &U, const flint::Vector &l,
                                             const flint::Vector &w, const flint::Complex &s, slong k)
{
    for (slong i = 0; i < k; ++i) {
        L.at(k, i).set(l[i]);
        U.at(i, k).set(w[i]);
    }

    L.at(k, k).set(1.0, 0.0);
    U.at(k, k).set(s);
}

void NestedSystemsSolver::solve_forward_substitution(flint::Vector &y, const flint::Matrix &L,
                                                     const flint::Matrix &rhs, slong size)
{
    flint::Complex temp, temp_sum;

    for (slong i = 0; i < size; ++i) {
        temp_sum.zero();
        for (slong j = 0; j < i; ++j) {
            flint::mul(temp, L.at(i, j), y[j], BYTE_PRECISION);
            flint::add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        flint::sub(y[i], rhs.at(i, 0), temp_sum, BYTE_PRECISION);
    }
}

void NestedSystemsSolver::solve_backward_substitution(flint::Vector &x, const flint::Vector &y,
                                                      const flint::Matrix &U, slong size)
{
    flint::Complex temp, temp_sum;

    for (slong i = size - 1; i >= 0; --i) {
        temp_sum.zero();
        for (slong j = i + 1; j < size; ++j) {
            flint::mul(temp, U.at(i, j), x[j], BYTE_PRECISION);
            flint::add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        flint::sub(x[i], y[i], temp_sum, BYTE_PRECISION);
        flint::div(x[i], x[i], U.at(i, i), BYTE_PRECISION);
    }
}

void NestedSystemsSolver::lu_solve_all()
{
    fill_matrix();
    fill_rhs();

    flint::Matrix L(max_system_size, max_system_size);
    flint::Matrix U(max_system_size, max_system_size);
    flint::Matrix P(max_system_size, max_system_size);

    flint::Vector x(1), y(1);
    initialize_identity_matrix(P);
    initialize_base_case(L, U, x, y);

    for (slong size = 2; size <= max_system_size; ++size) {
        slong k = size - 1;

        // Pivoting
        slong pivot_idx = find_pivot_index(k, matrix);
        swap_rows(matrix, rhs, L, P, k, pivot_idx);

        // LU decomposition
        flint::Vector u(k), w(k), v(k), l(k);
        compute_u_vector(u, k);
        compute_w_vector(w, u, L, k);
        compute_v_vector(v, k);
        compute_l_vector(l, v, U, k);

        flint::Complex s;
        compute_s(s, l, w, k);
        update_LU_matrices(L, U, l, w, s, k);

        // Solve system
        flint::Vector y(size), x(size);
        solve_forward_substitution(y, L, rhs, size);
        solve_backward_substitution(x, y, U, size);

        solutions.push_back(x);
    }
}

void NestedSystemsSolver::modified_gram_schmidt(flint::Matrix &Q, flint::Matrix &R, const flint::Matrix &A)
{
    slong n = A.nrows();

    if (A.ncols() != n) {
        throw ZetaException("Matrix must be square");
    }

    for (slong j = 0; j < n; j++) {
        flint::Vector v(n);

        for (slong i = 0; i < n; i++) {
            v[i].set(A.at(i, j));
        }

        for (slong i = 0; i < j; i++) {
            flint::Complex dot;
            flint::Vector qi(n);
            for (slong k = 0; k < n; k++) {
                qi[k].set(Q.at(k, i));
            }

            flint::dot(dot, qi, v, BYTE_PRECISION);
            R.at(i, j).set(dot);

            flint::Vector scaled_qi;
            flint::mul(scaled_qi, qi, dot, BYTE_PRECISION);
            flint::sub(v, v, scaled_qi, BYTE_PRECISION);
        }

        flint::Complex norm;
        flint::norm(norm, v, BYTE_PRECISION);
        R.at(j, j).set(norm);

        if (!norm.is_zero()) {
            for (slong i = 0; i < n; i++) {
                flint::div(Q.at(i, j), v[i], norm, BYTE_PRECISION);
            }
        }
    }
}

flint::Vector NestedSystemsSolver::solve_upper_triangular(const flint::Matrix &R, const flint::Vector &b)
{
    slong n = R.nrows();

    if (R.ncols() != n || b.size() != n) {
        throw ZetaException("Matrix must be square and vector must match the matrix size");
    }

    flint::Vector x(n);

    for (slong i = n - 1; i >= 0; i--) {
        flint::Complex diag(R.at(i, i));
        if (diag.is_zero()) {
            throw ZetaException("Matrix is singular or poorly conditioned");
        }

        x[i].set(b[i]);

        for (slong j = i + 1; j < n; j++) {
            flint::Complex temp;
            flint::mul(temp, R.at(i, j), x[j], BYTE_PRECISION);
            flint::sub(x[i], x[i], temp, BYTE_PRECISION);
        }

        flint::div(x[i], x[i], diag, BYTE_PRECISION);
    }

    return x;
}

flint::Vector NestedSystemsSolver::qr_solve_system(const flint::Matrix &A_k, const flint::Vector &b_k)
{
    slong k = A_k.nrows();

    if (A_k.ncols() != k || b_k.size() != k) {
        throw ZetaException("Matrix must be square and vector must match the matrix size");
    }

    flint::Matrix Q(k, k);
    flint::Matrix R(k, k);
    modified_gram_schmidt(Q, R, A_k);

    flint::Vector Q_T_b(k);
    for (slong i = 0; i < k; i++) {
        flint::Vector q_i(k);
        for (slong j = 0; j < k; j++) {
            q_i[j].set(Q.at(j, i));
        }
        dot(Q_T_b[i], q_i, b_k, BYTE_PRECISION);
    }

    return solve_upper_triangular(R, Q_T_b);
}

void prepare_subsystem(flint::Matrix &A_k, flint::Vector &b_k, const flint::Matrix &A, const flint::Vector &b, slong k)
{
    if (A.nrows() < k || A.ncols() < k || b.size() < k) {
        throw ZetaException("Requested submatrix size exceeds original dimensions");
    }

    A_k = flint::Matrix(k, k);
    b_k = flint::Vector(k);

    for (slong i = 0; i < k; i++) {
        b_k[i].set(b[i]);
        for (slong j = 0; j < k; j++) {
            A_k.at(i, j).set(A.at(i, j));
        }
    }
}

void NestedSystemsSolver::qr_solve_all()
{
    fill_matrix();
    fill_rhs();
    std::vector<std::future<flint::Vector>> futures;

    flint::Vector temp_rhs(rhs[0]->get(), max_system_size);

    for (slong k = 1; k <= max_system_size; k++) {
        futures.emplace_back(std::async(std::launch::async, [&, k]() {
            flint::Matrix A_k(k, k);
            flint::Vector b_k(k);
            prepare_subsystem(A_k, b_k, matrix, temp_rhs, k);
            return qr_solve_system(A_k, b_k);
        }));
    }

    for (auto &f : futures) {
        solutions.push_back(f.get());
    }
}

void NestedSystemsSolver::slow_solve_all()
{
    fill_matrix();
    fill_rhs();

    std::vector<std::future<flint::Vector>> futures;
    flint::Vector temp_rhs(rhs[0]->get(), max_system_size);

    for (slong k = 1; k <= max_system_size; k++) {
        futures.emplace_back(std::async(std::launch::async, [&, k]() {
            flint::Matrix x(k, 1);
            flint::Matrix A_k(k, k);
            flint::Vector b_k(k);
            prepare_subsystem(A_k, b_k, matrix, temp_rhs, k);

            flint::Matrix b_k_temp(b_k);
            acb_mat_solve(x.get(), A_k.get(), b_k_temp.get(), precision);

            return flint::Vector(x[0]->get(), k);
        }));
    }

    for (auto &f : futures) {
        solutions.push_back(f.get());
    }
}
