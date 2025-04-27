#include "acb_vector.hpp"

acb_vector::acb_vector(slong capacity) noexcept 
    : capacity(capacity * 2), size(capacity * 2) 
{
    data = _acb_vec_init(capacity * 2);
}

// Конструктор перемещения
acb_vector::acb_vector(acb_vector&& other) noexcept 
    : data(other.data), capacity(other.capacity), size(other.size) 
{
    other.data = nullptr;
    other.capacity = 0;
    other.size = 0;
}

// Деструктор
acb_vector::~acb_vector() 
{
    if (data != nullptr) 
    {
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

acb_ptr acb_vector::operator[](slong i) noexcept 
{
    return data + i;
}

slong acb_vector::get_size() const noexcept 
{
    return size;
}

void acb_vector::set_size(slong new_size) noexcept 
{
    if (new_size <= capacity) 
    {
        size = new_size;
    } 
    else 
    {
        size = capacity;
    }
}