#include "solver.hpp"
#include <vector>
#include <memory>

// RAII-обертка для acb_mat_t
struct AcbMat {
    acb_mat_t mat;
    AcbMat(slong r, slong c) { acb_mat_init(mat, r, c); }
    ~AcbMat() { acb_mat_clear(mat); }
    AcbMat(const AcbMat&) = delete;
    AcbMat& operator=(const AcbMat&) = delete;
};

void save_solutions(const char* filename, const std::vector<std::unique_ptr<AcbMat>>& solutions, slong max_m) { 
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    const slong print_digits = 15;
    
    for (slong m = 1; m <= max_m; ++m) {
        if (m-1 >= solutions.size()) break;
        
        flint_fprintf(file, "Solution for m=%wd:\n[", m);
        acb_mat_fprintd(file, solutions[m-1]->mat, print_digits);
        flint_fprintf(file, "]\n");
    }
    fclose(file);
}

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

void solve_all(const acb_vector& zeros, slong max_m, slong precision, const char* output_file) {
    std::vector<std::unique_ptr<AcbMat>> solutions;
    
    for (slong m = 1; m <= max_m; ++m) {
        // Исправленное обращение к размеру вектора
        if (m - 1 > zeros.get_size()) {
            flint_printf("Not enough zeros for m=%wd\n", m);
            continue;
        }

        acb_mat_t matrix, LU, rhs, sol;
        acb_mat_init(matrix, m, m);
        acb_mat_init(LU, m, m);
        acb_mat_init(rhs, m, 1);
        acb_mat_init(sol, m, 1);
        
        fill_matrix(matrix, m, zeros, precision);
        acb_mat_set(LU, matrix);
        
        slong* perm = (slong*)flint_malloc(sizeof(slong) * m);
        int result = acb_mat_lu(perm, LU, LU, precision);
        
        for (slong i = 0; i < m-1; ++i) acb_zero(acb_mat_entry(rhs, i, 0));
        acb_one(acb_mat_entry(rhs, m-1, 0));
        
        if (result) {
            acb_mat_solve_lu_precomp(sol, perm, LU, rhs, precision);
            auto solution = std::make_unique<AcbMat>(m, 1);
            acb_mat_set(solution->mat, sol);
            solutions.push_back(std::move(solution));
        }

        flint_free(perm);
        acb_mat_clear(matrix);
        acb_mat_clear(LU);
        acb_mat_clear(rhs);
        acb_mat_clear(sol);
    }
    
    if (output_file) save_solutions(output_file, solutions, max_m);
}