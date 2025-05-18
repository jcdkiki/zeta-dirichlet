#include "acb_vector.hpp"

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
