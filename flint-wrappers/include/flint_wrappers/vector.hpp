#ifndef FLINT_WRAPPERS_VECTOR_HPP
#define FLINT_WRAPPERS_VECTOR_HPP

#include "flint_wrappers/complex.hpp"
#include "flint_wrappers/common.hpp"
#include <flint/acb.h>
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

void dot(Complex &res, const Vector &v1, const Vector &v2, slong precision);

inline void norm(Complex &res, const Vector &vec, slong precision)
{
    dot(res, vec, vec, precision);
    sqrt(res, res, precision);
}

inline void mul(Vector &result, const Vector &vec, const Complex &scalar, slong precision)
{
#ifdef DEBUG
    if (result.size() != vec.size()) {
        throw FlintException("Vectors must be of the same size");
    }
#endif
    _acb_vec_scalar_mul(result.data(), vec.data(), result.size(), scalar.get(), precision);
}

inline void sub(Vector &result, const Vector &left, const Vector &right, slong precision)
{
#ifdef DEBUG
    if (left.size() != right.size() || result.size() != left.size()) {
        throw FlintException("Vectors must be of the same size");
    }
#endif
    _acb_vec_sub(result.data(), left.data(), right.data(), left.size(), precision);
}

} // namespace flint

#endif
