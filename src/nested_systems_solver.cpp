#include "../hdrs/nested_systems_solver.hpp"
#include <iostream>

NestedSystemsSolver::NestedSystemsSolver(std::vector<coefficient>& fixed_coefs, acb_vector& zeta_zeros, slong precision)
    : zeros(zeta_zeros), fixed_coefficients(fixed_coefs), precision(precision) {}

void NestedSystemsSolver::fill_initial_matrix(acb_matrix& matrix, slong current_system_size, slong current_n_zeros) {
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    slong n_fix_coefs = fixed_coefficients.size();

    for (slong i = 0; i < current_n_zeros; ++i) {
        for (slong j = 0; j < current_system_size; ++j) {
            acb_set_d_d(base, j + 1, 0.0);       // base = (j+1) + 0i
            acb_set(exp, zeros[i]);               // exp = zeros[i]
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision);   // res = base^-exp
            acb_set(acb_mat_entry(*matrix.get_mat(), i, j), res);
        }
    }

    for (slong i = 0; i < n_fix_coefs; ++i) {
        slong row = current_n_zeros + i;
        if (row >= current_system_size) break;
        slong col = fixed_coefficients[i].idx;
        if (col >= current_system_size) continue;
        acb_set_d_d(acb_mat_entry(*matrix.get_mat(), row, col), 1.0, 0.0);
    }

    acb_clear(base);
    acb_clear(exp);
    acb_clear(res);
}

void NestedSystemsSolver::update_matrix(acb_matrix& matrix, slong prev_n_zeros, slong current_n_zeros, slong current_system_size) {
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    for (slong i = prev_n_zeros; i < current_n_zeros; ++i) {
        for (slong j = 0; j < current_system_size; ++j) {
            acb_set_d_d(base, j + 1, 0.0);
            acb_set(exp, zeros[i]);
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision);
            acb_set(acb_mat_entry(*matrix.get_mat(), i, j), res);
        }
    }

    for (slong i = 0; i < prev_n_zeros; ++i) {
        for (slong j = prev_n_zeros; j < current_system_size; ++j) {
            acb_set_d_d(base, j + 1, 0.0);
            acb_set(exp, zeros[i]);
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision);
            acb_set(acb_mat_entry(*matrix.get_mat(), i, j), res);
        }
    }

    slong n_fix_coefs = fixed_coefficients.size();
    for (slong i = 0; i < n_fix_coefs; ++i) {
        slong row = current_n_zeros + i;
        if (row >= current_system_size) break;
        slong col = fixed_coefficients[i].idx;
        if (col >= current_system_size) continue;

        for (slong j = 0; j < current_system_size; ++j) {
            acb_zero(acb_mat_entry(*matrix.get_mat(), row, j));
        }

        acb_set_d_d(acb_mat_entry(*matrix.get_mat(), row, col), 1.0, 0.0);
    }

    acb_clear(base);
    acb_clear(exp);
    acb_clear(res);
}

void NestedSystemsSolver::fill_initial_rhs(acb_vector& rhs, slong current_system_size, slong current_n_zeros) {
    slong n_fixed_coeffs = fixed_coefficients.size();
    for (slong i = 0; i < current_system_size; ++i) {
        acb_zero(rhs[i]);
    }
    for (slong i = 0; i < n_fixed_coeffs; ++i) {
        slong row = current_n_zeros + i;
        if (row >= current_system_size) break;
        coefficient coef = fixed_coefficients[i];
        acb_set_d_d(rhs[row], coef.re_value, coef.im_value);
    }
}

void NestedSystemsSolver::update_rhs(acb_vector& rhs, slong prev_n_zeros, slong current_n_zeros, slong current_system_size) {
    for (slong i = prev_n_zeros; i < current_n_zeros; ++i) {
        acb_zero(rhs[i]);
    }

    slong n_fixed_coeffs = fixed_coefficients.size();
    for (slong i = 0; i < n_fixed_coeffs; ++i) {
        slong row = current_n_zeros + i;
        if (row >= current_system_size) break;
        coefficient coef = fixed_coefficients[i];
        acb_set_d_d(rhs[row], coef.re_value, coef.im_value);
    }
}

void NestedSystemsSolver::compute_initial_lu(acb_matrix& matrix, acb_matrix& L, acb_matrix& U, slong system_size) {
    for (slong i = 0; i < system_size; ++i) {
        for (slong j = 0; j < system_size; ++j) {
            acb_zero(acb_mat_entry(*L.get_mat(), i, j));
            acb_zero(acb_mat_entry(*U.get_mat(), i, j));
        }
        acb_set_d_d(acb_mat_entry(*L.get_mat(), i, i), 1.0, 0.0);
    }

    for (slong i = 0; i < system_size; ++i) {
        for (slong j = 0; j < system_size; ++j) {
            acb_set(acb_mat_entry(*U.get_mat(), i, j), acb_mat_entry(*matrix.get_mat(), i, j));
        }
    }

    acb_t tmp;
    acb_init(tmp);

    for (slong i = 0; i < system_size; ++i) {
        acb_ptr diag_i = acb_mat_entry(*U.get_mat(), i, i);
        if (acb_is_zero(diag_i)) {
            std::cerr << "Error: Zero pivot at (" << i << ", " << i << ")" << std::endl;
            return;
        }
        for (slong j = i + 1; j < system_size; ++j) {
            acb_ptr L_ji = acb_mat_entry(*L.get_mat(), j, i);
            acb_ptr U_ji = acb_mat_entry(*U.get_mat(), j, i);
            acb_div(L_ji, U_ji, diag_i, precision);
            acb_zero(U_ji);
            for (slong k = i + 1; k < system_size; ++k) {
                acb_ptr U_jk = acb_mat_entry(*U.get_mat(), j, k);
                acb_ptr U_ik = acb_mat_entry(*U.get_mat(), i, k);
                acb_mul(tmp, L_ji, U_ik, precision);
                acb_sub(U_jk, U_jk, tmp, precision);
            }
        }
    }

    acb_clear(tmp);
}

void NestedSystemsSolver::update_lu(acb_matrix& matrix, acb_matrix& L, acb_matrix& U, slong prev_size, slong new_size) {
    acb_t tmp;
    acb_init(tmp);

    for (slong i = prev_size; i < new_size; ++i) {
        for (slong j = 0; j < new_size; ++j) {
            acb_zero(acb_mat_entry(*L.get_mat(), i, j));
            acb_zero(acb_mat_entry(*U.get_mat(), i, j));
        }
        acb_set_d_d(acb_mat_entry(*L.get_mat(), i, i), 1.0, 0.0);
    }
    for (slong i = 0; i < prev_size; ++i) {
        for (slong j = prev_size; j < new_size; ++j) {
            acb_zero(acb_mat_entry(*L.get_mat(), i, j));
            acb_zero(acb_mat_entry(*U.get_mat(), i, j));
        }
    }

    for (slong i = 0; i < new_size; ++i) {
        for (slong j = prev_size; j < new_size; ++j) {
            acb_set(acb_mat_entry(*U.get_mat(), i, j), acb_mat_entry(*matrix.get_mat(), i, j));
        }
    }
    for (slong i = prev_size; i < new_size; ++i) {
        for (slong j = 0; j < prev_size; ++j) {
            acb_set(acb_mat_entry(*U.get_mat(), i, j), acb_mat_entry(*matrix.get_mat(), i, j));
        }
    }

    for (slong i = prev_size; i < new_size; ++i) {
        for (slong k = 0; k < i; ++k) {
            acb_ptr L_ik = acb_mat_entry(*L.get_mat(), i, k);
            acb_ptr U_kk = acb_mat_entry(*U.get_mat(), k, k);
            acb_ptr U_ik = acb_mat_entry(*U.get_mat(), i, k);
            if (!acb_is_zero(U_kk)) {
                acb_div(L_ik, U_ik, U_kk, precision);
            } else {
                acb_zero(L_ik);
            }
            for (slong j = k + 1; j < new_size; ++j) {
                acb_ptr U_ij = acb_mat_entry(*U.get_mat(), i, j);
                acb_ptr U_kj = acb_mat_entry(*U.get_mat(), k, j);
                acb_mul(tmp, L_ik, U_kj, precision);
                acb_sub(U_ij, U_ij, tmp, precision);
            }
        }
    }

    acb_clear(tmp);
}

void NestedSystemsSolver::solve_system(acb_matrix& L, acb_matrix& U, acb_vector& b, acb_vector& x, slong system_size) {
    acb_vector y(system_size);
    acb_t tmp;
    acb_init(tmp);

    for (slong i = 0; i < system_size; ++i) {
        acb_set(y[i], b[i]);
        for (slong j = 0; j < i; ++j) {
            acb_ptr L_ij = acb_mat_entry(*L.get_mat(), i, j);
            acb_submul(y[i], L_ij, y[j], precision);
        }
    }

    for (slong i = system_size - 1; i >= 0; --i) {
        acb_set(x[i], y[i]);
        for (slong j = i + 1; j < system_size; ++j) {
            acb_ptr U_ij = acb_mat_entry(*U.get_mat(), i, j);
            acb_submul(x[i], U_ij, x[j], precision);
        }
        acb_ptr U_ii = acb_mat_entry(*U.get_mat(), i, i);
        if (!acb_is_zero(U_ii)) {
            acb_div(x[i], x[i], U_ii, precision);
        } else {
            std::cerr << "Warning: Zero diagonal in U at " << i << std::endl;
            acb_zero(x[i]);
        }
        if (!acb_is_finite(x[i])) {
            std::cerr << "Warning: NaN/Inf in solution at index " << i << std::endl;
            acb_zero(x[i]);
        }
    }

    acb_clear(tmp);
}

int NestedSystemsSolver::find_current_fix_coeffs_num(slong current_n_zeros) {
    int counter = 0;
    for (slong i = 0; i < fixed_coefficients.size(); ++i) {
        const coefficient& coef = fixed_coefficients[i];
        if (coef.idx < current_n_zeros) counter++;
        else break;
    }
    return counter;
}

void NestedSystemsSolver::solve_all_nested() {
    slong n_zeros = zeros.get_size();
    acb_matrix matrix(max_system_size, max_system_size);
    acb_matrix L(max_system_size, max_system_size);
    acb_matrix U(max_system_size, max_system_size);
    acb_vector b(max_system_size);
    acb_vector x(max_system_size);

    slong prev_n_zeros = 0;
    slong prev_system_size = 0;

    for (slong current_n_zeros = 2; current_n_zeros <= n_zeros; current_n_zeros += 2) {
        slong current_n_fix_coefs = find_current_fix_coeffs_num(current_n_zeros);
        slong current_system_size = current_n_zeros + current_n_fix_coefs;

        if (prev_n_zeros == 0) 
        {
            fill_initial_matrix(matrix, current_system_size, current_n_zeros);
            fill_initial_rhs(b, current_system_size, current_n_zeros);
            compute_initial_lu(matrix, L, U, current_system_size);
        } 
        else 
        {
            update_matrix(matrix, prev_n_zeros, current_n_zeros, current_system_size);
            update_rhs(b, prev_n_zeros, current_n_zeros, current_system_size);
            update_lu(matrix, L, U, prev_system_size, current_system_size);
        }

        solve_system(L, U, b, x, current_system_size);

        acb_vector x_copy(current_system_size);
        
        for (slong i = 0; i < current_system_size; ++i) {
            acb_set(x_copy[i], x[i]);
        }
        solutions.push_back(x_copy);

        prev_n_zeros = current_n_zeros;
        prev_system_size = current_system_size;
    }
}

acb_vector& NestedSystemsSolver::get_coefs_vector(int idx) {
    return solutions[idx];
}