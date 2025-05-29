#ifndef FLINT_WRAPPERS_VECTOR_HPP
#define FLINT_WRAPPERS_VECTOR_HPP

#include "flint_wrappers/complex.hpp"
#include <flint/flint.h>

namespace flint {
class Vector {
  private:
    acb_ptr _data;
    slong _size;

  public:
    Vector(const acb_ptr _data, slong _size);
    Vector(const Vector &);
    explicit Vector(Vector &&other) noexcept;
    explicit Vector(slong size = 0) noexcept;

    Vector &operator=(const Vector &);
    Vector &operator=(Vector &&other) noexcept;

    acb_ptr data() const
    {
        return _data;
    };
    slong size() const noexcept
    {
        return _size;
    };

    // warning: very dirty
    Complex &operator[](slong i) noexcept
    {
        return *(Complex *)(_data + i);
    }
    const Complex &operator[](slong i) const noexcept
    {
        return *(const Complex *)(_data + i);
    }

    ~Vector();
};

void dot_product(Complex &res, const Vector &v1, const Vector &v2);
void vector_norm(Complex &norm, const Vector &v);
Vector subtract_vectors(const Vector &v1, const Vector &v2);
Vector scalar_multiply(const Vector &v, const Complex &scalar);
} // namespace flint

#endif
