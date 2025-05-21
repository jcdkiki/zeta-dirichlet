#include "../hdrs/nested_systems_solver.hpp"
#include <flint/arb.h>
#include <flint/fmpq.h>
#include <limits>
#include <iostream>

NestedSystemsSolver::NestedSystemsSolver(std::vector<coefficient>& fixed_coefs, acb_vector& zeta_zeros, slong precision)
    : zeros(zeta_zeros), fixed_coefficients(fixed_coefs), precision(precision), matrix(max_system_size, max_system_size), rhs(max_system_size, 1) {}

acb_vector& NestedSystemsSolver::get_coefs_vector(int idx) {
    return solutions[idx];
}

void NestedSystemsSolver::fill_matrix()
{
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    slong n_coefs = fixed_coefficients.size();
    slong n_zeros = zeros.get_size();

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

void NestedSystemsSolver::solve_all_nested()
{
    fill_matrix();
    fill_rhs();

    acb_matrix L(max_system_size, max_system_size);
    acb_matrix U(max_system_size, max_system_size);
    acb_matrix P(max_system_size, max_system_size);

    for (slong i = 0; i < max_system_size; ++i)
    {
        for (slong j = 0; j < max_system_size; ++j)
        {
            if (i == j) acb_one(acb_mat_entry(*P.get_mat(), i, j));
        }
    }
    
    // initialization for size = 1

    acb_set_d_d(acb_mat_entry(*L.get_mat(), 0, 0), 1.0, 0.0);
    acb_set(acb_mat_entry(*U.get_mat(), 0, 0), acb_mat_entry(*matrix.get_mat(), 0, 0));

    acb_vector y(1), x(1);
    
    // y = [b[0] / U[0][0]]
    acb_div(y[0], acb_mat_entry(*rhs.get_mat(), 0, 0), acb_mat_entry(*U.get_mat(), 0, 0), BYTE_PRECISION);
    // x = y[0]
    acb_set(x[0], y[0]);
    solutions.push_back(x);

    for (slong size = 2; size <= max_system_size; ++size)
    {   
        acb_t temp, temp_sum;
        acb_init(temp_sum);
        acb_init(temp);
        slong k = size - 1;

        // pivoting for numerical stability
        arb_t pivot;
        arb_init(pivot);
        acb_abs(pivot, acb_mat_entry(*matrix.get_mat(), k, k), BYTE_PRECISION);
        slong pivot_idx = k;
        for (slong i = 0; i < k + 1; ++i)
        {
            arb_t current;
            arb_init(current);
            acb_abs(current, acb_mat_entry(*matrix.get_mat(), i, k), BYTE_PRECISION);
            if (arb_gt(current, pivot))
            {
                arb_set(pivot, current);
                pivot_idx = i;
            }
        }

        acb_t swapping_buffer;
        acb_init(swapping_buffer);
        if (pivot_idx != k)
        {
            for (slong j = 0; j < max_system_size; ++j)
            {
                acb_set(swapping_buffer, acb_mat_entry(*matrix.get_mat(), k, j));
                acb_set(acb_mat_entry(*matrix.get_mat(), k, j), acb_mat_entry(*matrix.get_mat(), pivot_idx, j));
                acb_set(acb_mat_entry(*matrix.get_mat(), pivot_idx, j), swapping_buffer);
            }

            acb_set(swapping_buffer, acb_mat_entry(*rhs.get_mat(), k, 0));
            acb_set(acb_mat_entry(*rhs.get_mat(), k, 0), acb_mat_entry(*rhs.get_mat(), pivot_idx, 0));
            acb_set(acb_mat_entry(*rhs.get_mat(), k, 0), swapping_buffer);
            
            for (slong j = 0; j < max_system_size; ++j)
            {
                acb_set(swapping_buffer, acb_mat_entry(*L.get_mat(), k, j));
                acb_set(acb_mat_entry(*L.get_mat(), k, j), acb_mat_entry(*L.get_mat(), pivot_idx, j));
                acb_set(acb_mat_entry(*L.get_mat(), pivot_idx, j), swapping_buffer);
            }

            for (slong j = 0; j < max_system_size; ++j)
            {
                acb_set(swapping_buffer, acb_mat_entry(*P.get_mat(), k, j));
                acb_set(acb_mat_entry(*P.get_mat(), k, j), acb_mat_entry(*P.get_mat(), pivot_idx, j));
                acb_set(acb_mat_entry(*P.get_mat(), pivot_idx, j), swapping_buffer);
            }
        }

        // forward
        // u = A[:k, k]
        acb_vector u(k);
        for (slong i = 0; i < k; ++i)
        {
            acb_set(u[i], acb_mat_entry(*matrix.get_mat(), i, k));
        }

        acb_vector w(k);
        
        acb_zero(temp_sum);
        acb_zero(temp);
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

        // backward
        // v = A[k, :k]
        acb_vector v(k);
        for (slong i = 0; i < k; ++i)
        {
            acb_set(v[i], acb_mat_entry(*matrix.get_mat(), k, i));
        }

        acb_vector l(k);
    

        acb_zero(temp_sum);
        acb_zero(temp);
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
        
        // s = A[k][k] - sum(l[j]*w[j]) j = 1 ... k
        acb_t s;
        acb_init(s);
        acb_zero(temp_sum);
        acb_zero(temp);
        for (slong i = 0; i < k; ++i)
        {   
            acb_mul(temp, l[i], w[i], BYTE_PRECISION);
            acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
        }
        acb_sub(s, acb_mat_entry(*matrix.get_mat(), k, k), temp_sum, BYTE_PRECISION);

        // update L and U
        for (slong i = 0; i < k; ++i)
        {
            acb_set(acb_mat_entry(*L.get_mat(), k, i), l[i]);
            acb_set(acb_mat_entry(*U.get_mat(), i, k), w[i]);
        }
        acb_set_d_d(acb_mat_entry(*L.get_mat(), k, k), 1.0, 0.0);
        acb_set(acb_mat_entry(*U.get_mat(), k, k), s);

        // solve L * y = b
        // forward
        acb_vector y(size);
        acb_zero(temp_sum);
        acb_zero(temp);
        for (slong i = 0; i < size; ++i)
        {
            acb_zero(temp_sum);
            for (slong j = 0; j < i; ++j)
            {
                acb_mul(temp, acb_mat_entry(*L.get_mat(), i, j), y[j], BYTE_PRECISION);
                acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
            }
            acb_sub(y[i], acb_mat_entry(*rhs.get_mat(), i, 0), temp_sum, BYTE_PRECISION);
        }

        acb_vector x(size);

        acb_zero(temp_sum);
        acb_zero(temp);
        for (slong i = size - 1; i >= 0; --i)
        {
            acb_zero(temp_sum);
            for (slong j = i + 1; j < size; ++j)
            {
                acb_mul(temp, acb_mat_entry(*U.get_mat(), i, j), x[j], BYTE_PRECISION);
                acb_add(temp_sum, temp_sum, temp, BYTE_PRECISION);
            }
            acb_sub(x[i], y[i], temp_sum, BYTE_PRECISION);
            acb_div(x[i], x[i], acb_mat_entry(*U.get_mat(), i, i), BYTE_PRECISION);
        }

        
        solutions.push_back(x);
        acb_clear(temp);
        acb_clear(temp_sum);
        acb_clear(s);
        arb_clear(pivot);
        acb_clear(swapping_buffer);
    }
}
