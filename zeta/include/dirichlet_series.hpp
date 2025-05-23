#ifndef DIRICHLET_SERIES_HPP
#define DIRICHLET_SERIES_HPP

#include <acb_wrappers/vector.hpp>

class DirichletSeries
{
private:
    acb::Vector coefficients;
public:
    DirichletSeries(acb::Vector& series_coefs);

    void calculate(acb_t result, acb_t X, slong precision);
};

#endif
