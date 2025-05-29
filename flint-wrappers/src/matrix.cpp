#include "flint_wrappers/matrix.hpp"
#include "flint_wrappers/complex.hpp"
#include <flint/acb_mat.h>
#include <iomanip>

namespace flint {

Matrix::Matrix(slong r, slong c)
{
    acb_mat_init(mat, r, c);
    acb_mat_zero(mat);
}

Matrix::Matrix(Matrix &&other) noexcept
{
    acb_mat_swap(mat, other.mat);
    acb_mat_init(other.mat, 0, 0);
}

Matrix &Matrix::operator=(Matrix &&other) noexcept
{
    if (this != &other) {
        acb_mat_swap(mat, other.mat);
        acb_mat_clear(other.mat);
        acb_mat_init(other.mat, 0, 0);
    }
    return *this;
}

Matrix::~Matrix()
{
    acb_mat_clear(mat);
}

Matrix::Matrix(const Vector &vec)
{
    acb_mat_init(mat, vec.size(), 1);

    for (slong i = 0; i < vec.size(); ++i) {
        acb_set(acb_mat_entry(mat, i, 0), vec[i].get());
    }
}

std::ostream &operator<<(std::ostream &os, const Matrix &mat)
{
    // TODO: replace with acb::Real
    arb_t real_part;
    arb_init(real_part);

    slong rows = mat.nrows();
    slong cols = mat.ncols();
    for (slong i = 0; i < rows; ++i) {
        for (slong j = 0; j < cols; ++j) {
            acb_get_real(real_part, acb_mat_entry(mat.get(), i, j));

            double midpoint_double = arf_get_d(arb_midref(real_part), ARF_RND_NEAR);

            // TODO: reset precision and width to prev value?
            os.precision(15);
            os << std::fixed << std::setw(15) << midpoint_double << " ";
        }
        os << std::endl;
    }

    arb_clear(real_part);
    return os;
}

Vector matrix_vector_multiply(const Matrix &M, const Vector &v, slong precision)
{
    if (M.ncols() != v.size()) {
        throw std::invalid_argument("Matrix columns must match vector size");
    }

    Vector result(M.nrows());

    for (slong i = 0; i < M.nrows(); i++) {
        Complex dot;

        for (slong j = 0; j < M.ncols(); j++) {
            Complex temp;
            mul(temp, M.at(i, j), v[j], precision);
            add(dot, dot, temp, precision);
        }

        result[i] = dot;
    }

    return result;
}

} // namespace flint
