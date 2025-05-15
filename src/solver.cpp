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

void lu_decomposition(acb_matrix& mat, slong n, slong precision) 
{
    acb_vector diagonal(n);

    for (slong i = 0; i < n; ++i)
    {
        acb_ptr diag_i = acb_mat_entry(*mat.get_mat(), i, i);

        if (i == 0) acb_set(diagonal[i], diag_i);
        else acb_mul(diagonal[i], diag_i, diagonal[i-1], precision);

        for (slong j = i + 1; j < n; ++j)
        {
            acb_ptr M_ji = acb_mat_entry(*mat.get_mat(), j, i);
            acb_div(M_ji, M_ji, diag_i, precision);
            acb_neg(M_ji, M_ji);

            for (slong k = i + 1; k < n; ++k) 
            {
                acb_ptr M_jk = acb_mat_entry(*mat.get_mat(), j, k);
                acb_ptr M_ik = acb_mat_entry(*mat.get_mat(), i, k);
                acb_addmul(M_jk, M_ik, M_ji, precision);
            }
        }
    }
}

void forward_substitution(const acb_matrix& L, acb_vector& y, const acb_vector& b, slong n, slong precision) 
{
    for (slong i = 0; i < n; ++i) 
    {
        acb_set(y[i], b[i]);

        for (slong j = 0; j < i; ++j) 
        {
            acb_ptr L_ij = acb_mat_entry(*L.get_mat(), i, j);
            acb_submul(y[i], L_ij, y[j], precision);
        }
    }
}

void backward_substitution(const acb_matrix& U, acb_vector& x, const acb_vector& y, slong n, slong precision) 
{
    for (slong i = n - 1; i >= 0; --i) 
    {
        acb_set(x[i], y[i]);

        for (slong j = i + 1; j < n; ++j) 
        {
            acb_ptr U_ij = acb_mat_entry(*U.get_mat(), i, j);
            acb_submul(x[i], U_ij, x[j], precision);
        }

        acb_ptr U_ii = acb_mat_entry(*U.get_mat(), i, i);
        acb_div(x[i], x[i], U_ii, precision);
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

    acb_vector b(system_size);

    // первые 2*n нули 
    for (slong i = 0; i < n_zeros; ++i) 
    {
        acb_zero(b[i]);
    }

    // последние n_fix_coefs фиксированные коэффициенты
    for (slong i = 0; i < n_fix_coefs; ++i) 
    {
        slong row = n_zeros + i;
        coefficient coef = fixated_coefficients[i];
        acb_set_d_d(b[row], coef.re_value, coef.im_value);
    }

    // Выполнить декомпозицию LU
    lu_decomposition(matrix, system_size, precision);

    // Решиние с помощью прямой и обратной подстановки
    acb_vector y(system_size);
    acb_vector x(system_size);

    forward_substitution(matrix, y, b, system_size, precision);
    backward_substitution(matrix, x, y, system_size, precision);

    // вот здесь нужна проверка на правильность решения перед копированием
    
    // Скопируйте решение в res
    for (slong i = 0; i < system_size; ++i) 
    {
        acb_set(acb_mat_entry(*res.get_mat(), i, 0), x[i]);
    }
}