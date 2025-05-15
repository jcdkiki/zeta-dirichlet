#include "../hdrs/solver.hpp"
#include "../hdrs/acb_matrix.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

void save_solutions(const char* filename, const std::vector<std::unique_ptr<acb_matrix>>& solutions, slong max_m) 
{ 
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    for (slong m = 1; m <= max_m; m += 2)
    {
        if ((m-1)/2 >= solutions.size()) break;
        
        flint_fprintf(file, "Solution for m=%wd:\n", m);

        for (slong i = 0; i < m; ++i) 
        {
            acb_fprintd(file, acb_mat_entry(*solutions[(m-1)/2]->get_mat(), i, 0), N_PRECISION);
            flint_fprintf(file, "\n");
        }
    }
    fclose(file);
}

void fill_matrix(acb_matrix& matrix, std::vector<coefficient>& fixated_coefficients, const acb_vector& zeros, slong precision) 
{
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    slong n_zeros = zeros.get_size();

    slong n_fix_coefs = fixated_coefficients.size();

    slong system_size = n_zeros + n_fix_coefs;


    for (slong i = 0; i < system_size; ++i) {
        for (slong j = 0; j < system_size; ++j) {
            acb_zero(acb_mat_entry(*matrix.get_mat(), i, j));
        }
    }

    // Заполняем первые 2n строк и оставляем n_fix_coefs пустыми (всего 2n + n_fix_coefs)
    for (slong i = 0; i < n_zeros; ++i) {
        for (slong j = 0; j < system_size; ++j) {
            acb_set_d_d(base, j + 1, 0.0);       // base = (j+1) + 0i
            acb_set(exp, zeros.get_ptr(i));       // exp = zeros[i]
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision);  // res = base^-exp
            acb_set(acb_mat_entry(*matrix.get_mat(), i, j), res);
        }
    }

    for (slong i = 0; i < n_fix_coefs; ++i) {
        int row = n_zeros + i;
        int col = fixated_coefficients[i].idx;

        acb_set_d_d(acb_mat_entry(*matrix.get_mat(), row, col), 1.0, 0.0);
    }

    acb_clear(base);
    acb_clear(exp);
    acb_clear(res);
}

static void lu_decomposition(acb_mat_t mat, slong n, slong precision) {
    std::vector<acb_struct> DIAGONAL(n);
    for (slong i = 0; i < n; ++i) {
        acb_init(&DIAGONAL[i]);
    }

    for (slong i = 0; i < n; ++i) {
        acb_ptr diag_i = acb_mat_entry(mat, i, i);

        if (i == 0) {
            acb_set(&DIAGONAL[i], diag_i);
        } else {
            acb_mul(&DIAGONAL[i], diag_i, &DIAGONAL[i-1], precision);
        }

        for (slong j = i + 1; j < n; ++j) {
            acb_ptr M_ji = acb_mat_entry(mat, j, i);
            acb_div(M_ji, M_ji, diag_i, precision);
            acb_neg(M_ji, M_ji);

            for (slong k = i + 1; k < n; ++k) {
                acb_ptr M_jk = acb_mat_entry(mat, j, k);
                acb_ptr M_ik = acb_mat_entry(mat, i, k);
                acb_addmul(M_jk, M_ik, M_ji, precision);
            }
        }
    }

    for (slong i = 0; i < n; ++i) {
        acb_clear(&DIAGONAL[i]);
    }
}

static void forward_substitution(const acb_mat_t L, acb_ptr y, const acb_ptr b, slong n, slong precision) {
    for (slong i = 0; i < n; ++i) {
        acb_set(y + i, b + i);
        for (slong j = 0; j < i; ++j) {
            acb_ptr L_ij = acb_mat_entry(L, i, j);
            acb_submul(y + i, L_ij, y + j, precision);
        }
    }
}

static void backward_substitution(const acb_mat_t U, acb_ptr x, const acb_ptr y, slong n, slong precision) {
    for (slong i = n - 1; i >= 0; --i) {
        acb_set(x + i, y + i);
        for (slong j = i + 1; j < n; ++j) {
            acb_ptr U_ij = acb_mat_entry(U, i, j);
            acb_submul(x + i, U_ij, x + j, precision);
        }
        acb_ptr U_ii = acb_mat_entry(U, i, i);
        acb_div(x + i, x + i, U_ii, precision);
    }
}

void solve(acb_matrix& res, const acb_vector& zeros, std::vector<coefficient>& fixated_coefficients, slong precision)
{
    slong n_zeros = zeros.get_size();

    slong n_fix_coefs = fixated_coefficients.size();

    slong system_size = n_zeros + n_fix_coefs;

    acb_matrix matrix(system_size, system_size);

    fill_matrix(matrix, fixated_coefficients, zeros, precision);

    acb_matrix b(system_size, 1);

    // первые 2*n нули 
    for (slong i = 0; i < n_zeros; ++i) 
    {
        acb_zero(acb_mat_entry(*b.get_mat(), i, 0));
    }

    // последние n_fix_coefs фиксированные коэффициенты
    for (slong i = 0; i < n_fix_coefs; ++i) 
    {
        slong row = n_zeros + i;
        coefficient coef = fixated_coefficients[i];
        acb_set_d_d(acb_mat_entry(*b.get_mat(), row, 0), coef.re_value, coef.im_value);
    }

    acb_matrix x(system_size, 1);

    // equation: matrix * x = b
    // shape: (2n+fix x 2n+fix) * (2n+fix x 1) = (2n+fix x 1)
    if (!acb_mat_solve(*x.get_mat(), *matrix.get_mat(), *b.get_mat(), precision)) 
    {
        printf("Система вырождена или не имеет решения!\n");
    } 
    else
    {
        res = std::move(x);
    }
}

void solve_all(acb_matrix& res, const acb_vector& zeros, std::vector<coefficient>& fixated_coefficients, slong precision)
{
    slong n_zeros = zeros.get_size();
    slong n_fix_coefs = fixated_coefficients.size();
    slong system_size = n_zeros + n_fix_coefs;

    acb_matrix matrix(system_size, system_size);
    fill_matrix(matrix, fixated_coefficients, zeros, precision);

    acb_matrix b(system_size, 1);

    // первые 2*n нули 
    for (slong i = 0; i < n_zeros; ++i) 
    {
        acb_zero(acb_mat_entry(*b.get_mat(), i, 0));
    }

    // последние n_fix_coefs фиксированные коэффициенты
    for (slong i = 0; i < n_fix_coefs; ++i) 
    {
        slong row = n_zeros + i;
        coefficient coef = fixated_coefficients[i];
        acb_set_d_d(acb_mat_entry(*b.get_mat(), row, 0), coef.re_value, coef.im_value);
    }

    // Выполнить декомпозицию LU
    acb_mat_t* mat = matrix.get_mat();
    lu_decomposition(*mat, system_size, precision);

    // Решиние с помощью прямой и обратной подстановки
    acb_ptr y = _acb_vec_init(system_size);
    acb_ptr x = _acb_vec_init(system_size);

    // Проверка решения на NaN/Inf
    int has_nan = 0;
    for (slong i = 0; i < system_size; ++i) {
        if (!acb_is_finite(x + i)) {
            std::cout << "[ERROR] Invalid solution at index " << i << std::endl;
            has_nan = 1;
        }
    }
    
    if (has_nan) {
        std::cout << "[ERROR] Solution contains invalid values\n" << std::endl;
        _acb_vec_clear(y, system_size);
        _acb_vec_clear(x, system_size);
        return;
    }

    forward_substitution(*mat, y, acb_mat_entry(*b.get_mat(), 0, 0), system_size, precision);
    backward_substitution(*mat, x, y, system_size, precision);

    // Скопируйте решение в res
    for (slong i = 0; i < system_size; ++i) {
        acb_set(acb_mat_entry(*res.get_mat(), i, 0), x + i);
    }

    _acb_vec_clear(y, system_size);
    _acb_vec_clear(x, system_size);
}