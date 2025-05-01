#include "solver.hpp"

void make_matrix(acb_mat_t &matrix, slong matrix_size, acb_vector &zeros,
                 slong precision)
{
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    // Заполняем первые 2n строк и оставляем последнюю пустой (всего 2n + 1)
    for (slong i = 0; i < matrix_size - 1; ++i) {
        for (slong j = 0; j < matrix_size; ++j) {
            acb_set_d_d(base, j + 1, 0.0);  // base = (j+1) + 0i
            acb_set(exp, zeros.get_ptr(i)); // exp = zeros[i]
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision); // res = base^-exp
            acb_set(acb_mat_entry(matrix, i, j), res);
        }
    }

    // Заполняем последнюю строку: [1, 0, 0, ..., 0]
    for (slong j = 0; j < matrix_size; ++j) {
        if (j == 0) {
            acb_set_d_d(acb_mat_entry(matrix, matrix_size - 1, j), 1.0, 0.0);
        }
        else {
            acb_zero(acb_mat_entry(matrix, matrix_size - 1, j));
        }
    }

    acb_clear(base);
    acb_clear(exp);
    acb_clear(res);
}

void solve(acb_mat_t &a, slong n, slong precision)
{
    acb_mat_t b;
    acb_mat_init(b, n, 1);

    for (slong j = 0; j < n; ++j) {
        if (j == n - 1) {
            acb_set_d_d(acb_mat_entry(b, j, 0), 1.0, 0.0);
        }
        else {
            acb_zero(acb_mat_entry(b, j, 0));
        }
    }

    acb_mat_t x;
    acb_mat_init(x, n, 1);

    if (!acb_mat_solve(x, a, b, precision)) {
        printf("Система вырождена или не имеет решения!\n");
    }
    else {
        printf("Решение x:\n");
        acb_mat_printd(x, 10);
    }

    acb_mat_clear(b);
    acb_mat_clear(x);
}
