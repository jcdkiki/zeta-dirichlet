#ifndef FLINT_WRAPPERS_COMPLEX_HPP
#define FLINT_WRAPPERS_COMPLEX_HPP

#include "flint_wrappers/real.hpp"

#include <flint/acb.h>
#include <flint/acb_types.h>
#include <gmp.h>

namespace flint {
class Complex {
    acb_t acb;

  public:
    Complex() { acb_init(acb); }
    Complex(double real) : Complex() { acb_set_d(acb, real); }
    explicit Complex(double real, double imag) : Complex() { acb_set_d_d(acb, real, imag); }

    inline void set(double real) { acb_set_d(acb, real); }
    inline void set(double real, double imag) { acb_set_d_d(acb, real, imag); }
    inline void set(const Real &real, const Real &imag) { acb_set_arb_arb(acb, real.get(), imag.get()); }
    inline void set(const Complex &other) { acb_set(acb, other.acb); }

    inline Real &real() { return *(Real*)(&acb->real); }
    inline const Real &real() const { return *(const Real*)(&acb->real); }
    inline Real &imag() { return *(Real*)(&acb->imag); }
    inline const Real &imag() const { return *(const Real*)(&acb->imag); }

    inline bool is_zero() { return acb_is_zero(acb); }

    acb_ptr get() { return acb; }
    acb_srcptr get() const { return acb; }

    inline void zero() { acb_zero(acb); }
    inline void one() { acb_one(acb); }

    ~Complex()
    {
        acb_clear(acb);
    }

    friend void add(Complex &result, const Complex &left, const Complex &right, slong precision);
    friend void sub(Complex &result, const Complex &left, const Complex &right, slong precision);
    friend void mul(Complex &result, const Complex &left, const Complex &right, slong precision);
    friend void div(Complex &result, const Complex &left, const Complex &right, slong precision);
    friend void pow(Complex &result, const Complex &base, const Complex &power, slong precision);
    friend void neg(Complex &result, const Complex &x);
    friend void abs(Real &result, const Complex &x, slong precision);
    friend void sqrt(Complex &result, const Complex &x, slong precision);

    inline void neg() { acb_neg(acb, acb); }
};

inline void add(Complex &result, const Complex &left, const Complex &right, slong precision)
{
    acb_add(result.acb, left.acb, right.acb, precision);
}

inline void sub(Complex &result, const Complex &left, const Complex &right, slong precision)
{
    acb_sub(result.acb, left.acb, right.acb, precision);
}

inline void mul(Complex &result, const Complex &left, const Complex &right, slong precision)
{
    acb_mul(result.acb, left.acb, right.acb, precision);
}

inline void div(Complex &result, const Complex &left, const Complex &right, slong precision)
{
    acb_div(result.acb, left.acb, right.acb, precision);
}

inline void pow(Complex &result, const Complex &base, const Complex &power, slong precision)
{
    acb_pow(result.acb, base.acb, power.acb, precision);
}

inline void neg(Complex &result, const Complex &x)
{
    acb_neg(result.acb, x.acb);
}

inline void abs(Real &result, const Complex &x, slong precision)
{
    acb_abs(result.get(), x.acb, precision);
}

inline void sqrt(Complex &result, const Complex &x, slong precision)
{
    acb_sqrt(result.acb, x.acb, precision);
}

} // namespace flint

#endif
