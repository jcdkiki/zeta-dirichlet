#include "../hdrs/acb_matrix.hpp"
#include <iostream>
#include <iomanip>
#include "../hdrs/utils.hpp"

acb_matrix::acb_matrix(slong r, slong c)
    : rows(r), cols(c)
{
    acb_mat_init(mat, r, c);
    for (slong i = 0; i < r; ++i) {
        for (slong j = 0; j < c; ++j) {
            acb_zero(acb_mat_entry(mat, i, j));
        }
    }
}

acb_matrix::acb_matrix(acb_matrix&& other) noexcept
    : rows(other.rows), cols(other.cols)
{
    acb_mat_swap(mat, other.mat);
    other.rows = 0;
    other.cols = 0;
    acb_mat_init(other.mat, 0, 0);
}

acb_matrix& acb_matrix::operator=(acb_matrix&& other) noexcept
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

acb_mat_t* acb_matrix::get_mat() noexcept
{
    return &mat;
}

const acb_mat_t* acb_matrix::get_mat() const noexcept
{
    return &mat;
}

slong acb_matrix::row_count() const noexcept
{
    return rows;
}

slong acb_matrix::col_count() const noexcept
{
    return cols;
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

void print_matrix(acb_matrix matrix, slong matrix_size)
{
    arb_t real_part;
    arb_init(real_part);
    for (slong i = 0; i < matrix_size; ++i)
    {
        for (slong j = 0; j < matrix_size; ++j)
        {
            acb_get_real(real_part, acb_mat_entry(*matrix.get_mat(), i, j));

            double midpoint_double = arf_get_d(arb_midref(real_part), ARF_RND_NEAR);
            std::cout.precision(N_PRECISION);
            std::cout << std::fixed << std::setw(15) << midpoint_double << " ";
        }
        std::cout << std::endl;
    }
    arb_clear(real_part);
}
