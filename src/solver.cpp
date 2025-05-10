#include "solver.hpp"
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>

// RAII-обертка для acb_mat_t
struct AcbMat 
{
    acb_mat_t mat;
    AcbMat(slong r, slong c) { acb_mat_init(mat, r, c); }
    ~AcbMat() { acb_mat_clear(mat); }
    AcbMat(const AcbMat&) = delete;
    AcbMat& operator=(const AcbMat&) = delete;
};

void save_solutions(const char* filename, const std::vector<std::unique_ptr<AcbMat>>& solutions, slong max_m) 
{ 
    FILE* file = fopen(filename, "w");
    if (!file) return;
    
    const slong print_digits = 15;
    
    for (slong m = 1; m <= max_m; m += 2)
    {
        if ((m-1)/2 >= solutions.size()) break;
        
        flint_fprintf(file, "Solution for m=%wd:\n", m);

        for (slong i = 0; i < m; ++i) 
        {
            acb_fprintd(file, acb_mat_entry(solutions[(m-1)/2]->mat, i, 0), print_digits);
            flint_fprintf(file, "\n");
        }
    }
    fclose(file);
}

void compute_series(acb_t result, const acb_t X, int m, const std::map<int, acb_vector>& solutions, slong precision) 
{
    auto it = solutions.find(m);

    if (it == solutions.end()) 
    {
        throw std::runtime_error("compute_series: m not found in solutions");
    }

    const acb_vector& coeffs = it->second;
    slong num_coeffs = coeffs.get_size();

    if (num_coeffs != m) 
    {
        throw std::runtime_error("compute_series: number of coefficients does not match m");
    }

    acb_zero(result);

    acb_t base, exponent, term, pow_result;
    acb_init(base);
    acb_init(exponent);
    acb_init(term);
    acb_init(pow_result);

    acb_neg(exponent, X); // exponent = -s

    for (slong j = 0; j < num_coeffs; ++j) 
    {
        slong current_base = j + 1;
        acb_set_si(base, current_base);
        acb_pow(pow_result, base, exponent, precision);
        acb_mul(term, coeffs.get_ptr(j), pow_result, precision);
        acb_add(result, result, term, precision);
    }

    acb_clear(base);
    acb_clear(exponent);
    acb_clear(term);
    acb_clear(pow_result);
}

void fill_matrix(acb_mat_t matrix, slong m, const acb_vector& zeros, slong precision) 
{
    for (slong i = 0; i < m - 1; ++i) 
    {
        for (slong j = 0; j < m; ++j) 
        {
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
    for (slong j = 0; j < m; ++j) 
    {
        if (j == 0) 
        {
            acb_one(acb_mat_entry(matrix, m - 1, j));
        } 
        else 
        {
            acb_zero(acb_mat_entry(matrix, m - 1, j));
        }
    }
}

void solve_all(const acb_vector& zeros, slong max_m, slong precision, const char* output_file) 
{
    std::vector<std::unique_ptr<AcbMat>> solutions;
    
    if (max_m % 2 == 0) 
    {
        flint_printf("Error: max_m must be odd (2n+1), got %wd\n", max_m);
        return;
    }

    slong num_zeros = zeros.get_size();
    slong max_valid_m = 2 * num_zeros + 1;
    
    if (max_m > max_valid_m) 
    {
        max_m = max_valid_m;
        flint_printf("Adjusted max_m to %wd (2*%wd+1)\n", max_m, num_zeros);
    }

    for (slong m = 1; m <= max_m; m += 2) 
    {
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
        
        if (result) 
        {
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