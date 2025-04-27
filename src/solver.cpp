#include "solver.hpp"

void make_full_matrix(acb_mat_t& matrix, slong n, acb_vector& zeros, slong precision) {
    acb_t base, exp, res;
    acb_init(base);
    acb_init(exp);
    acb_init(res);

    if (zeros.get_size() < n - 1) {
        throw std::runtime_error("Not enough zeros in vector");
    }

    // Заполняем первые n-1 строк
    for (slong i = 0; i < n - 1; ++i) {
        for (slong j = 0; j < n; ++j) {
            acb_set_d_d(base, j + 1, 0.0);       // base = (j+1) + 0i
            acb_set(exp, zeros.get_ptr(i));       // exp = zeros[i]
            acb_neg(exp, exp);
            acb_pow(res, base, exp, precision);  // res = base^-exp
            acb_set(acb_mat_entry(matrix, i, j), res);
        }
    }

    // Последняя строка: [1, 0, ..., 0]
    for (slong j = 0; j < n; ++j) {
        if (j == 0) {
            acb_set_d_d(acb_mat_entry(matrix, n - 1, j), 1.0, 0.0);
        } else {
            acb_zero(acb_mat_entry(matrix, n - 1, j));
        }
    }

    acb_clear(base);
    acb_clear(exp);
    acb_clear(res);
}

void solve_all(acb_mat_t& A, slong n, slong precision) {
    slong* perm = (slong*)flint_malloc(sizeof(slong) * n);

    for (slong m = 1; m <= n; ++m) {
        // Создаем временную матрицу для текущего m
        acb_mat_t LU;
        acb_mat_init(LU, m, m);
        
        // Копируем подматрицу m x m из исходной матрицы A
        for (slong i = 0; i < m; ++i) {
            for (slong j = 0; j < m; ++j) {
                acb_set(acb_mat_entry(LU, i, j), 
                       acb_mat_entry(A, i, j));
            }
        }

        // LU-разложение для текущего размера m
        int result = acb_mat_lu(perm, LU, LU, precision);

        if (!result) {
            flint_printf("System for m=%wd is singular\n", m);
            acb_mat_clear(LU);
            continue;
        }

        // Решение системы
        acb_mat_t b, x;
        acb_mat_init(b, m, 1);
        acb_mat_init(x, m, 1);

        for (slong i = 0; i < m - 1; ++i) {
            acb_zero(acb_mat_entry(b, i, 0));
        }
        acb_one(acb_mat_entry(b, m - 1, 0));

        acb_mat_solve_lu_precomp(x, perm, LU, b, precision);

        // Вывод
        flint_printf("Solution for m=%wd:\n", m);
        acb_mat_printd(x, 10);
        flint_printf("\n");

        // Очистка временных данных
        acb_mat_clear(b);
        acb_mat_clear(x);
        acb_mat_clear(LU);
    }

    flint_free(perm);
}