#include "acb_wrappers/matrix.hpp"
#include <iomanip>
#include <iostream>

acb_matrix::acb_matrix(slong r, slong c) : rows(r), cols(c)
{
    acb_mat_init(mat, r, c);
    for (slong i = 0; i < r; ++i) {
        for (slong j = 0; j < c; ++j) {
            acb_zero(acb_mat_entry(mat, i, j));
        }
    }
}

acb_matrix::acb_matrix(acb_matrix &&other) noexcept
    : rows(other.rows), cols(other.cols)
{
    acb_mat_swap(mat, other.mat);
    other.rows = 0;
    other.cols = 0;
    acb_mat_init(other.mat, 0, 0);
}

acb_matrix &acb_matrix::operator=(acb_matrix &&other) noexcept
{
    if (this != &other) {
        acb_mat_clear(mat);

        rows = other.rows;
        cols = other.cols;

        acb_mat_swap(mat, other.mat);
        other.rows = 0;
        other.cols = 0;
        acb_mat_init(other.mat, 0, 0);
    }
    return *this;
}

acb_matrix::~acb_matrix()
{
    acb_mat_clear(mat);
}

acb_ptr acb_matrix::operator()(slong row)
{
    if (row < 0 || row >= rows) {
        throw std::out_of_range("acb_matrix: row index out of range");
    }
    return acb_mat_entry(mat, row, 0);
}

const acb_ptr acb_matrix::operator()(slong row) const
{
    if (row < 0 || row >= rows) {
        throw std::out_of_range("acb_matrix: row index out of range");
    }
    return acb_mat_entry(mat, row, 0);
}

acb_matrix::acb_matrix(acb::Vector &vec)
{
    acb_mat_init(mat, vec.size(), 1);

    for (slong i = 0; i < vec.size(); ++i) {
        acb_set(acb_mat_entry(mat, i, 0), vec[i]);
    }
}

void print_matrix(acb_matrix matrix, slong matrix_size)
{
    arb_t real_part;
    arb_init(real_part);
    for (slong i = 0; i < matrix_size; ++i) {
        for (slong j = 0; j < matrix_size; ++j) {
            acb_get_real(real_part, acb_mat_entry(*matrix.get_mat(), i, j));

            double midpoint_double =
                arf_get_d(arb_midref(real_part), ARF_RND_NEAR);
            
            std::cout.precision(15);
            std::cout << std::fixed << std::setw(15) << midpoint_double << " ";
        }
        std::cout << std::endl;
    }
    arb_clear(real_part);
}

acb::Vector matrix_vector_multiply(const acb_matrix &M, const acb::Vector &v)
{
    if (M.col_count() != v.size()) {
        throw std::invalid_argument("Matrix columns must match vector size");
    }

    acb::Vector result(M.row_count());

    for (slong i = 0; i < M.row_count(); i++) {
        acb_t dot;
        acb_init(dot);

        for (slong j = 0; j < M.col_count(); j++) {
            acb_t temp;
            acb_init(temp);
            acb_mul(temp, acb_mat_entry(*M.get_mat(), i, j), v[j], 64);
            acb_add(dot, dot, temp, 64);
            acb_clear(temp);
        }

        acb_set(result[i], dot);
        acb_clear(dot);
    }

    return result;
}
