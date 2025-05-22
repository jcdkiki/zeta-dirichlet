#include "dirichlet_series.hpp"
#include "utils.hpp"

void calculate_dirichlet_series(const acb::Vector &coefficients, acb_t result, acb_t X, slong precision)
{
    acb_zero(result);

    acb_t pow, base, exp;
    acb_init(pow);
    acb_init(base);
    acb_init(exp);

    acb_neg(exp, X); // -x

    for (slong i = 0; i < coefficients.size(); ++i)
    {
        acb_set_si(base, i + 1);
        acb_pow(pow, base, exp, precision); // (i + 1) ^ -X
        acb_mul(pow, pow, coefficients[i], precision); // a_n * (i + 1) ^ -X

        acb_add(result, result, pow, precision);
    }

    acb_clear(base);
    acb_clear(pow);
    acb_clear(exp);
}
