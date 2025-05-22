#ifndef ACB_VECTOR_HPP
#define ACB_VECTOR_HPP

#include <flint/acb.h>
#include <stdexcept>
#include "acb_matrix.hpp"

class acb_vector {
  private:
    acb_ptr data;
    slong capacity;
    slong size;

  public:
    acb_vector(const acb_ptr vec, slong size);
    acb_vector(const acb_vector &);
    acb_vector &operator=(const acb_vector &) = delete;

    acb_vector &operator=(acb_vector&& other) noexcept;

public:
    explicit acb_vector(slong capacity) noexcept;

    explicit acb_vector(acb_vector &&other) noexcept;

    acb_ptr get_ptr(slong i) const;

    slong get_size() const noexcept;

    void set_size(slong new_size) noexcept;

    acb_ptr operator[](slong i) const noexcept;

    ~acb_vector();
};

void dot_product(acb_t res, const acb_vector &v1, const acb_vector &v2);
void vector_norm(acb_t norm, const acb_vector &v);
acb_vector subtract_vectors(const acb_vector &v1, const acb_vector &v2);
acb_vector scalar_multiply(const acb_vector &v, const acb_t &scalar);
acb_vector matrix_vector_multiply(const acb_matrix &M, const acb_vector &v);

#endif
