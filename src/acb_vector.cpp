#include "../hdrs/acb_vector.hpp"

acb_vector::acb_vector(slong capacity) noexcept 
    : capacity(capacity), size(capacity) 
{
    data = _acb_vec_init(capacity);
}

acb_vector::acb_vector(const acb_ptr vec, slong size)
{
    data = _acb_vec_init(size);
    for (slong i = 0; i < size; ++i)
    {
        _acb_vec_set(data, vec, size);
    }
    this->capacity = size;
    this->size = size;
}

// Конструктор перемещения
acb_vector::acb_vector(acb_vector &&other) noexcept
    : data(other.data), capacity(other.capacity), size(other.size)
{
    other.data = nullptr;
    other.capacity = 0;
    other.size = 0;
}

acb_vector& acb_vector::operator=(acb_vector&& other) noexcept {
    if (this != &other) {
        if (data != nullptr) {
            _acb_vec_clear(data, capacity);
        }

        data = other.data;
        capacity = other.capacity;
        size = other.size;

        other.data = nullptr;
        other.capacity = 0;
        other.size = 0;
    }
    return *this;
}

// Деструктор
acb_vector::~acb_vector()
{
    if (data != nullptr) {
        _acb_vec_clear(data, capacity);
    }
}

acb_ptr acb_vector::get_ptr(slong i) const
{
    if (i < 0 || i >= size) {
        throw std::out_of_range("acb_vector: index out of range");
    }
    return data + i;
}

acb_ptr acb_vector::operator[](slong i) const noexcept
{
    return data + i;
}

slong acb_vector::get_size() const noexcept
{
    return size;
}

void acb_vector::set_size(slong new_size) noexcept
{
    if (new_size <= capacity) {
        size = new_size;
    }
    else {
        size = capacity;
    }
}
