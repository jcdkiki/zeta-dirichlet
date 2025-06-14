#include "nested_systems_solver.hpp"
#include "common.hpp"
#include <flint_wrappers/matrix.hpp>
#include "flint_wrappers/complex.hpp"
#include <future>
#include <iostream>
#include <limits>

// TODO: fix meeeee
static constexpr int BYTE_PRECISION = 2048;

NestedSystemsSolver::NestedSystemsSolver(slong mat_size, std::vector<coefficient> &fixed_coefs,
                                         flint::Vector &zeta_zeros, slong precision)
    : zeros(zeta_zeros), fixed_coefficients(fixed_coefs), precision(precision),
      matrix(mat_size, mat_size), rhs(mat_size, 1), max_system_size(mat_size)
{
}

flint::Vector &NestedSystemsSolver::get_coefs_vector(int idx)
{
    return solutions[idx];
}

void NestedSystemsSolver::fill_matrix_nofix()
{
    flint::Complex base, exp, res;
    for (slong i = 0; i < max_system_size; ++i) 
    {
        for (slong j = 0; j < max_system_size; ++j) 
        {
            base.set((double)(j + 1), 0.0);         // base = (col+1) + 0i
            exp.set(zeros[i]);                      // exp = zeros[zero_ptr]
            exp.neg();                              // exp = -exp
            flint::pow(res, base, exp, precision);  // res = base^-exp            
            matrix.at(j, i).set(res);
        }
    }
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

void NestedSystemsSolver::modified_gram_schmidt(flint::Matrix &Q, flint::Matrix &R, const flint::Matrix &A)
{
    slong n = A.nrows();

    if (A.ncols() != n) {
        throw ZetaException("Matrix must be square");
    }

    for (slong j = 0; j < n; j++) 
    {
        flint::Vector v(n);

        for (slong i = 0; i < n; i++) 
        {
            v[i].set(A.at(i, j));
        }

        for (slong i = 0; i < j; i++) 
        {
            flint::Complex dot;
            flint::Vector qi(n);
            for (slong k = 0; k < n; k++) 
            {
                qi[k].set(Q.at(k, i));
            }

            flint::dot(dot, qi, v, precision);
            R.at(i, j).set(dot);

            flint::Vector scaled_qi(n);
            flint::mul(scaled_qi, qi, dot, precision);
            flint::sub(v, v, scaled_qi, precision);
        }

        flint::Complex norm;
        flint::norm(norm, v, precision);
        R.at(j, j).set(norm);

        if (!norm.is_zero()) {
            for (slong i = 0; i < n; i++) {
                flint::div(Q.at(i, j), v[i], norm, precision);
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
            flint::mul(temp, R.at(i, j), x[j], precision);
            flint::sub(x[i], x[i], temp, precision);
        }

        flint::div(x[i], x[i], diag, precision);
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
        dot(Q_T_b[i], q_i, b_k, precision);
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

    for (slong k = 1; k <= max_system_size; k++) 
    {
        futures.emplace_back(std::async(std::launch::async, [&, k]() 
        {
            if (k < 50) precision *= 1;
            else if (k < 100) precision *= 2;
            else if (k < 150) precision *= 2;
            else if (k < 300) precision *= 2;
            
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
        futures.emplace_back(std::async(std::launch::async, [&, k]() 
        {
            if (k < 50) precision = 1024;
            else if (k < 100) precision = 2048;
            else if (k < 150) precision = 4096;
            else if (k < 300) precision = 8192;

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

void NestedSystemsSolver::compute_lu_decomposition(flint::Matrix &L, flint::Vector &diagonal) 
{
    flint::Complex temp;

    flint::Vector diag(max_system_size + 1);

    for (slong i = 0; i < max_system_size; ++i) 
    {
        diag[i].set(matrix.at(i, i));
        diagonal[i].set(diag[i]);

        if (i > 0)
        {
            flint::mul(diagonal[i], diagonal[i], diagonal[i - 1], precision);
        }

        for (slong j = i + 1; j < max_system_size; ++j) 
        {
            flint::Complex z;
            flint::div(z, matrix.at(j, i), diag[i], precision);
            z.neg();
            L.at(j, i).set(z);
            matrix.at(j, i).set(z);

            for (slong k = i + 1; k < max_system_size; ++k) 
            {
                flint::Complex temp;
                flint::mul(temp, matrix.at(i, k), matrix.at(j, i), precision);
                flint::add(matrix.at(j, k), matrix.at(j, k), temp, precision);
                L.at(j, k).set(matrix.at(j, k));
            }
        }
    }

    for (slong k = 0; k < max_system_size; ++k) 
    {
        for (slong i = k + 1; i < max_system_size; ++i) 
        {
            for (slong j = i + 1; j < max_system_size; ++j) 
            {
                flint::Complex temp;
                flint::mul(temp, L.at(i, k), matrix.at(j, i), precision);
                flint::add(L.at(j, k), L.at(j, k), temp, precision);
            }
        }
    }
}

void NestedSystemsSolver::optimized_lu_solve_all() 
{
    fill_matrix_nofix();

    flint::Matrix L(max_system_size, max_system_size);
    flint::Vector diagonal(max_system_size + 1);
    
    slong init_precision = precision;
    precision = 8192;
    compute_lu_decomposition(L, diagonal);

    precision = init_precision;
    
    for (slong current_size = 1; current_size <= max_system_size; ++current_size) 
    {
        if (current_size < 50) precision = 1024;
        else if (current_size < 100) precision = 2048;
        else if (current_size < 150) precision = 4096;
        else if (current_size < 300) precision = 8192;

        std::vector<coefficient> current_fixed_coeffs;
        for (const auto& fix_coef: fixed_coefficients)
        {
            if (fix_coef.idx < current_size)
            {
                current_fixed_coeffs.push_back(fix_coef);
            } 
        }

        flint::Vector res1 = lu_solve_system(current_size, L, diagonal);
        solutions.push_back(std::move(res1));
    }
}

flint::Vector NestedSystemsSolver::lu_solve_system(slong size, const flint::Matrix& L, const flint::Vector& diagonal) 
{
    flint::Matrix current_solution(size + 1, size + 1);
    flint::Vector inverse(size + 1);

    for (slong j = 1; j <= size; ++j)
    {
        for (slong k = 1; k < j; ++k) 
        {
            current_solution.at(j, k).set(L.at(j - 1, k - 1));
            if (j > 2)
            {
                flint::mul(current_solution.at(j, k), current_solution.at(j, k), diagonal[j - 2], precision);
            }
        }
        
        current_solution.at(j, j).one();
        if (j > 1)
        {
            current_solution.at(j, j).set(diagonal[j - 1]);
        }

        flint::Complex temp;
        temp.one();
        flint::div(inverse[j], temp, current_solution.at(j, 1), precision);
        
        for (slong k = 1; k <= j; ++k) 
        {
            flint::mul(current_solution.at(j, k), current_solution.at(j, k), inverse[j], precision);
        }
    }

    flint::Vector res(size);

    for (slong i = 0; i < size; ++i) 
    {   
        res[i].set(current_solution.at(size, i + 1));
    }

    return res;
}

