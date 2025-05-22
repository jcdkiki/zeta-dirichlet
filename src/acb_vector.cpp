#include "../hdrs/acb_vector.hpp"
#include "../hdrs/utils.hpp"

acb_vector::acb_vector(slong capacity) noexcept 
    : capacity(capacity), size(capacity) 
{
    data = _acb_vec_init(capacity);
}

acb_vector::acb_vector(const acb_ptr vec, slong size)
{
    this->data = _acb_vec_init(size);
    for (slong i = 0; i < size; ++i)
    {
        _acb_vec_set(data, vec, size);
    }
    this->capacity = size;
    this->size = size;
}

acb_vector::acb_vector(const acb_vector & other)
{
    this->capacity = other.capacity;
    this->size = other.size;
    this->data = _acb_vec_init(other.capacity);

    for (slong i = 0; i < size; ++i)
    {
        _acb_vec_set(data, other.data, size);
    }
}

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


void dot_product(acb_t res, const acb_vector &v1, const acb_vector &v2) 
{
    if (v1.get_size() != v2.get_size()) 
    {
        throw std::invalid_argument("Vectors must be of the same size");
    }

    acb_zero(res);
    for (slong i = 0; i < v1.get_size(); i++)
    {
        acb_t temp;
        acb_init(temp);
        acb_mul(temp, v1[i], v2[i], BYTE_PRECISION);  
        acb_add(res, res, temp, BYTE_PRECISION);
        acb_clear(temp);
    }
}

void vector_norm(acb_t norm, const acb_vector &v) 
{
    acb_t dot;
    acb_init(dot);

    dot_product(dot, v, v);
    acb_sqrt(norm, dot, BYTE_PRECISION);

    acb_clear(dot);
}

acb_vector subtract_vectors(const acb_vector &v1, const acb_vector &v2) 
{
    if (v1.get_size() != v2.get_size()) 
    {
        throw std::invalid_argument("Vectors must be of the same size");
    }

    acb_vector result(v1.get_size());

    for (slong i = 0; i < v1.get_size(); i++) 
    {
        acb_sub(result[i], v1[i], v2[i], BYTE_PRECISION);
    }

    return result;
}

acb_vector scalar_multiply(const acb_vector &v, const acb_t &scalar) 
{
    acb_vector result(v.get_size());

    for (slong i = 0; i < v.get_size(); i++)
    {
        acb_mul(result[i], v[i], scalar, BYTE_PRECISION);
    }

    return result;
}

acb_vector matrix_vector_multiply(const acb_matrix &M, const acb_vector &v) {
    if (M.col_count() != v.get_size()) {
        throw std::invalid_argument("Matrix columns must match vector size");
    }

    acb_vector result(M.row_count());

    for (slong i = 0; i < M.row_count(); i++) {
        acb_t dot;
        acb_init(dot);

        for (slong j = 0; j < M.col_count(); j++) {
            acb_t temp;
            acb_init(temp);
            acb_mul(temp, acb_mat_entry(*M.get_mat(), i, j), v[j], 64);
            acb_add(dot, dot, temp, 64);
            acb_clear(temp);
        }

        acb_set(result[i], dot);
        acb_clear(dot);
    }

    return result;
}