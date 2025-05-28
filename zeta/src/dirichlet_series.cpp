#include "dirichlet_series.hpp"

void DirichletSeries::calculate(flint::Complex &result, const flint::Complex &X, slong precision)
{
    result = 0.0;
    neg(exp, X); // -x

    for (slong i = 0; i < coefficients.size(); ++i) {
        base = (double)(i + 1); // TODO: acb_set_si
        flint::pow(pow, base, exp, precision); // (i + 1) ^ -X
        flint::mul(pow, pow, coefficients[i], precision); // a_n * (i + 1) ^ -X
        flint:add(result, result, pow, precision);
    }
}
