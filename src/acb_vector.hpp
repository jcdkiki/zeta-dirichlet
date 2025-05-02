#ifndef ACB_VECTOR_HPP
#define ACB_VECTOR_HPP

#include <flint/acb.h>
#include <stdexcept>

class acb_vector {
  private:
    acb_ptr data;
    slong capacity;
    slong size;

  public:
    acb_vector(const acb_vector &) = delete;
    acb_vector &operator=(const acb_vector &) = delete;

    acb_vector& operator=(acb_vector&& other) noexcept;

public:
    explicit acb_vector(slong capacity) noexcept;

    explicit acb_vector(acb_vector &&other) noexcept;

    acb_ptr get_ptr(slong i) const;

    slong get_size() const noexcept;

    void set_size(slong new_size) noexcept;

    acb_ptr operator[](slong i) noexcept;

    ~acb_vector();
};

#endif
