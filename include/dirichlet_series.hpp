#ifndef DIRICHLET_SERIES_HPP
#define DIRICHLET_SERIES_HPP

#include "acb_vector.hpp"

void calculate_dirichlet_series(const acb::Vector &coefficients, acb_t result, acb_t X, slong precision);

#endif
