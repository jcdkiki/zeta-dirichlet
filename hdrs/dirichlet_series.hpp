#ifndef DIRICHLET_SERIES_HPP
#define DIRICHLET_SERIES_HPP

#include "acb_vector.hpp"

class dirichlet_series
{
private:
    acb_vector coefficients;
public:
    dirichlet_series(acb_vector& series_coefs);

    void calculate(acb_t result, acb_t X, slong precision);
};

#endif