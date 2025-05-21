#ifndef DIRICHLET_SERIES_HPP
#define DIRICHLET_SERIES_HPP

#include "acb_vector.hpp"

class DirichletSeries
{
private:
    acb_vector coefficients;
public:
    DirichletSeries(acb_vector& series_coefs);

    void calculate(acb_t result, acb_t X, slong precision);
};

#endif