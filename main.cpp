#include "hdrs/reader.hpp"
#include "hdrs/solver.hpp"
#include "hdrs/utils.hpp"
#include "hdrs/dirichlet_series.hpp"
#include <vector>

int main()
{
    acb_vector zeros(2 * N_ZEROS);
    read_zeros(zeros, PATH, N_ZEROS, BYTE_PRECISION);
    
    std::vector<coefficient> fixed_coeficients = 
    {
        coefficient(0, 1.0, 0.0),
    };

    slong system_size = 2 * N_ZEROS + fixed_coeficients.size();

    // в res сохраним полученные коэффициенты
    acb_matrix res(system_size, 1);
    
    solve_all(res, zeros, fixed_coeficients, BYTE_PRECISION);

    acb_vector coeffs(res(0), system_size);

    // конечный ряд с заданными коэффициентами
    dirichlet_series series(coeffs);

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