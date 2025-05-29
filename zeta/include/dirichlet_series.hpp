#ifndef DIRICHLET_SERIES_HPP
#define DIRICHLET_SERIES_HPP

#include <flint_wrappers/vector.hpp>

class DirichletSeries {
  private:
    flint::Vector  coefficients;
    flint::Complex pow, base, exp;

  public:
    DirichletSeries(flint::Vector &coefficients) : coefficients(coefficients) {}

    void calculate(flint::Complex &result, const flint::Complex &X, slong precision);
};

#endif
