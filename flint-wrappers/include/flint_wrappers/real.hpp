#ifndef FLINT_WRAPPERS_REAL_HPP
#define FLINT_WRAPPERS_REAL_HPP

#include <flint/arb.h>

namespace flint {
    class Real {
        arb_t arb;
    public:
        Real() { arb_init(arb); }
        Real(double x) : Real() { arb_set_d(arb, x); }

        inline void set(double x) { arb_set_d(arb, x); }
        inline int set(const char *str, slong precision) { return arb_set_str(arb, str, precision); }
        inline void set(const Real &other) { arb_set(arb, other.arb); }

        inline arf_struct &mid() { return arb->mid; }
        inline const arf_struct &mid() const { return arb->mid; }
        inline mag_struct &rad() { return arb->rad; }
        inline const mag_struct &rad() const { return arb->rad; }

        inline arb_struct *get() { return arb; }
        inline const arb_struct *get() const { return arb; }

        friend void add(Real &result, const Real &left, const Real &right, slong precision);
        friend void sub(Real &result, const Real &left, const Real &right, slong precision);
        friend void mul(Real &result, const Real &left, const Real &right, slong precision);
        friend void pow(Real &result, const Real &base, const Real &power, slong precision);
        friend void neg(Real &result, const Real &x);
        friend void sqrt(Real &result, const Real &x, slong precision);

        inline bool operator>(const Real &other) { return arb_gt(arb, other.arb); }
        inline bool operator>=(const Real &other) { return arb_ge(arb, other.arb); }
        inline bool operator==(const Real &other) { return arb_eq(arb, other.arb); }
        inline bool operator<(const Real &other) { return arb_lt(arb, other.arb); }
        inline bool operator<=(const Real &other) { return arb_le(arb, other.arb); }
    };

    inline void add(Real &result, const Real &left, const Real &right, slong precision)
    {
        arb_add(result.arb, left.arb, right.arb, precision);
    }

    inline void sub(Real &result, const Real &left, const Real &right, slong precision)
    {
        arb_sub(result.arb, left.arb, right.arb, precision);
    }

    inline void mul(Real &result, const Real &left, const Real &right, slong precision)
    {
        arb_mul(result.arb, left.arb, right.arb, precision);
    }

    inline void pow(Real &result, const Real &base, const Real &power, slong precision)
    {
        arb_pow(result.arb, base.arb, power.arb, precision);
    }

    inline void neg(Real &result, const Real &x)
    {
        arb_neg(result.arb, x.arb);
    }

    inline void sqrt(Real &result, const Real &x, slong precision)
    {
        arb_sqrt(result.arb, x.arb, precision);
    }
}

#endif
