#ifndef FLINT_WRAPPERS_MATRIX_HPP
#define FLINT_WRAPPERS_MATRIX_HPP

#include "flint_wrappers/vector.hpp"
#include <flint/acb_mat.h>
#include <ostream>

namespace flint {
class Matrix {
  private:
    acb_mat_t mat;

  public:
    Matrix(const Matrix &) = delete;
    Matrix &operator=(const Matrix &) = delete;

    Matrix(slong r, slong c);
    Matrix(const Vector &vec);
    Matrix(Matrix &&other) noexcept;
    Matrix &operator=(Matrix &&other) noexcept;

    ~Matrix();

    inline void zero() { acb_mat_zero(mat); }

    acb_mat_struct *get() noexcept
    {
        return mat;
    }
    const acb_mat_struct *get() const noexcept
    {
        return mat;
    };

    slong nrows() const noexcept
    {
        return acb_mat_nrows(mat);
    }
    slong ncols() const noexcept
    {
        return acb_mat_ncols(mat);
    }

    // we won't check bounds for efficency.
    // dirty btw.
    // TODO: add checks in debug mode????
    Complex *operator[](slong row)
    {
        return (Complex *)acb_mat_entry(mat, row, 0);
    }
    const Complex *operator[](slong row) const
    {
        return (const Complex *)acb_mat_entry(mat, row, 0);
    }
    Complex &at(int i, int j)
    {
        return *(Complex *)acb_mat_entry(mat, i, j);
    }
    const Complex &at(int i, int j) const
    {
        return *(const Complex *)acb_mat_entry(mat, i, j);
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix &mat);

    // friend void mul(Vector &result, const Vector &v);
};

void mul(Vector &result, const Matrix &M, const Vector &v, slong precision);
} // namespace flint

#endif
