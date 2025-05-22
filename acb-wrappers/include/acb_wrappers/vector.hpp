#ifndef ACB_WRAPPERS_VECTOR_HPP
#define ACB_WRAPPERS_VECTOR_HPP

#include <flint/flint.h>
#include <flint/acb.h>

namespace acb {
    class Vector {
    private:
        acb_ptr _data;
        slong _size;

    public:
        Vector(const acb_ptr _data, slong _size);
        Vector(const Vector&);
        explicit Vector(Vector &&other) noexcept;
        explicit Vector(slong size = 0) noexcept;
        
        Vector &operator=(const Vector &);
        Vector &operator=(Vector&& other) noexcept;

        acb_ptr data() const { return _data; };
        slong size() const noexcept { return _size; };
        acb_ptr operator[](slong i) const noexcept { return _data + i; };

        ~Vector();
    };
}

void dot_product(acb_t res, const acb::Vector &v1, const acb::Vector &v2);
void vector_norm(acb_t norm, const acb::Vector &v);
acb::Vector subtract_vectors(const acb::Vector &v1, const acb::Vector &v2);
acb::Vector scalar_multiply(const acb::Vector &v, const acb_t &scalar);

#endif
