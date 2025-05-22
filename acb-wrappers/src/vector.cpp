#include "acb_wrappers/vector.hpp"
#include "acb_wrappers/common.hpp"

// TODO: hmmmmmm
static constexpr int BYTE_PRECISION = 2048;

namespace acb {
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

    Vector::Vector(Vector &&other) noexcept
        : _data(other._data), _size(other._size)
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


    Vector& Vector::operator=(Vector&& other) noexcept {
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
}

void dot_product(acb_t res, const acb::Vector &v1, const acb::Vector &v2) 
{
    if (v1.size() != v2.size()) {
        throw AcbException("Vectors must be of the same size");
    }

    acb_zero(res);
    for (slong i = 0; i < v1.size(); i++) {
        acb_t temp;
        acb_init(temp);
        acb_mul(temp, v1[i], v2[i], BYTE_PRECISION);  
        acb_add(res, res, temp, BYTE_PRECISION);
        acb_clear(temp);
    }
}

void vector_norm(acb_t norm, const acb::Vector &v) 
{
    acb_t dot;
    acb_init(dot);

    dot_product(dot, v, v);
    acb_sqrt(norm, dot, BYTE_PRECISION);

    acb_clear(dot);
}

acb::Vector subtract_vectors(const acb::Vector &v1, const acb::Vector &v2) 
{
    if (v1.size() != v2.size()) {
        throw AcbException("Vectors must be of the same size");
    }

    acb::Vector result(v1.size());

    for (slong i = 0; i < v1.size(); i++) {
        acb_sub(result[i], v1[i], v2[i], BYTE_PRECISION);
    }

    return result;
}

acb::Vector scalar_multiply(const acb::Vector &v, const acb_t &scalar) 
{
    acb::Vector result(v.size());

    for (slong i = 0; i < v.size(); i++) {
        acb_mul(result[i], v[i], scalar, BYTE_PRECISION);
    }

    return result;
}
