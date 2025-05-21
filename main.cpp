#include "hdrs/reader.hpp"
#include "hdrs/solver.hpp"
#include "hdrs/utils.hpp"
#include "hdrs/dirichlet_series.hpp"
#include "hdrs/nested_systems_solver.hpp"
#include <vector>

int main()
{
    acb_vector zeros(2 * N_ZEROS);
    read_zeros(zeros, PATH, N_ZEROS, BYTE_PRECISION);
    
    std::vector<coefficient> fixed_coeficients = 
    {
        coefficient(0, 1.0, 0.0),
    };

    slong system_size = zeros.get_size() + fixed_coeficients.size();
    // acb_matrix res(system_size, 1);
    // solve(res, zeros, fixed_coeficients, BYTE_PRECISION);

    NestedSystemsSolver ns_solver(fixed_coeficients, zeros);
    
    ns_solver.solve_all_nested();

    acb_vector coeffs = ns_solver.get_coefs_vector(49);
    std::cout << coeffs.get_size() << std::endl;

    for (int i = 0; i < coeffs.get_size(); ++i)
    {
        acb_printd(coeffs[i], N_PRECISION);
        std::cout<<std::endl;
    }

    // конечный ряд с заданными коэффициентами
    // DirichletSeries series(ns_solver.get_coefs_vector(0));
    // пример вычисления ряда в точке
    // можно расскоментировать
    // acb_t r;
    // acb_init(r);

    // acb_t X;
    // acb_init(X);
    // acb_set_d_d(X, 1.0, 0.0);
    // series.calculate(r, X, BYTE_PRECISION);
    // acb_printd(r, N_PRECISION);
    // std::cout<<std::endl;

    // acb_clear(r);
    // acb_clear(X);

    return 0;
}