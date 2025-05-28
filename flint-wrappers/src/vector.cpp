#include "flint_wrappers/vector.hpp"
#include "flint_wrappers/common.hpp"
#include "flint_wrappers/complex.hpp"

// TODO: hmmmmmm
static constexpr int BYTE_PRECISION = 2048;

namespace flint {
Vector::Vector(slong _size) noexcept : _size(_size)
{
    _data = (_size == 0) ? nullptr : _acb_vec_init(_size);
}

Vector::Vector(const acb_ptr _data, slong _size)
{
    this->_data = _acb_vec_init(_size);
    this->_size = _size;
    _acb_vec_set(this->_data, _data, _size);
}

Vector::Vector(Vector &&other) noexcept : _data(other._data), _size(other._size)
{
    other._data = nullptr;
    other._size = 0;
}

Vector &Vector::operator=(const Vector &other)
{
    if (this != &other) {
        if (_data != nullptr) {
            _acb_vec_clear(_data, _size);
        }

        _data = _acb_vec_init(other._size);
        _size = other._size;
        _acb_vec_set(_data, other._data, _size);
    }

    return *this;
}

Vector::Vector(const Vector &other)
{
    _data = _acb_vec_init(other._size);
    _size = other._size;
    _acb_vec_set(_data, other._data, _size);
}

Vector &Vector::operator=(Vector &&other) noexcept
{
    if (this != &other) {
        if (_data != nullptr) {
            _acb_vec_clear(_data, _size);
        }

        _data = other._data;
        _size = other._size;

        other._data = nullptr;
        other._size = 0;
    }

    return *this;
}

Vector::~Vector()
{
    if (_data != nullptr) {
        _acb_vec_clear(_data, _size);
    }
}

void dot_product(Complex &res, const Vector &v1, const Vector &v2)
{
    if (v1.size() != v2.size()) {
        throw FlintException("Vectors must be of the same size");
    }

    res = 0;
    for (slong i = 0; i < v1.size(); i++) {
        Complex temp;
        mul(temp, v1[i], v2[i], BYTE_PRECISION);
        add(res, res, temp, BYTE_PRECISION);
    }
}

void vector_norm(Complex &norm, const Vector &v)
{
    Complex dot;
    dot_product(dot, v, v);
    sqrt(norm, dot, BYTE_PRECISION);
}

Vector subtract_vectors(const Vector &v1, const Vector &v2)
{
    if (v1.size() != v2.size()) {
        throw FlintException("Vectors must be of the same size");
    }

    Vector result(v1.size());

    for (slong i = 0; i < v1.size(); i++) {
        sub(result[i], v1[i], v2[i], BYTE_PRECISION);
    }

    return result;
}

Vector scalar_multiply(const Vector &v, const Complex &scalar)
{
    Vector result(v.size());

    for (slong i = 0; i < v.size(); i++) {
        mul(result[i], v[i], scalar, BYTE_PRECISION);
    }

    return result;
}
} // namespace flint
