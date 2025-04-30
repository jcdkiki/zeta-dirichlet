#include "solver.hpp"

void fill_matrix(acb_mat_t matrix, slong m, const acb_vector& zeros, slong precision) {
    // Заполнение первых m-1 строк
    for (slong i = 0; i < m - 1; ++i) {
        for (slong j = 0; j < m; ++j) {
            acb_t base, exp, res;
            acb_init(base);
            acb_init(exp);
            acb_init(res);

            acb_set_d_d(base, j + 1, 0.0);  // base = j + 1
            acb_set(exp, zeros.get_ptr(i));       // exp = zeros[i]
            acb_neg(exp, exp);         // exp = -s_i
            acb_pow(res, base, exp, precision);  // res = (j+1)^(-s_i)
            acb_set(acb_mat_entry(matrix, i, j), res);

            acb_clear(base);
            acb_clear(exp);
            acb_clear(res);
        }
    }

    // Последняя строка [1, 0, ..., 0]
    for (slong j = 0; j < m; ++j) {
        if (j == 0) {
            acb_one(acb_mat_entry(matrix, m - 1, j));
        } else {
            acb_zero(acb_mat_entry(matrix, m - 1, j));
        }
    }
}

// Основная функция решения систем
void solve_all(const acb_vector& zeros, slong max_m, slong precision) {
    for (slong m = 1; m <= max_m; ++m) {
        if (m - 1 > zeros.get_size()) {
            flint_printf("Not enough zeros for m=%wd\n", m);
            continue;
        }

        // Инициализация матрицы
        acb_mat_t matrix;
        acb_mat_init(matrix, m, m);
        fill_matrix(matrix, m, zeros, precision);  // Заполняем матрицу

        // LU-разложение
        slong* perm = (slong*)flint_malloc(sizeof(slong) * m);
        acb_mat_t LU;
        acb_mat_init(LU, m, m);
        acb_mat_set(LU, matrix);

        int result = acb_mat_lu(perm, LU, LU, precision);

        // Подготовка правой части [0, ..., 0, 1]
        acb_mat_t rhs, sol;
        acb_mat_init(rhs, m, 1);
        acb_mat_init(sol, m, 1);

        for (slong i = 0; i < m - 1; ++i) {
            acb_zero(acb_mat_entry(rhs, i, 0));
        }
        acb_one(acb_mat_entry(rhs, m - 1, 0));

        // Решение системы и вывод
        if (!result) {
            flint_printf("System for m=%wd is singular\n", m);
        } else {
            acb_mat_solve_lu_precomp(sol, perm, LU, rhs, precision);
            flint_printf("Solution for m=%wd:\n[", m);
            acb_mat_printd(sol, 10);
        }

        flint_free(perm);
        acb_mat_clear(LU);
        acb_mat_clear(rhs);
        acb_mat_clear(sol);
        acb_mat_clear(matrix);
    }
}