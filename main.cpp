#include "src/reader.hpp"
#include "src/solver.hpp"
#include "src/reader_solutions.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 6;
constexpr char path[] = "/mnt/1DE0B2F616840AF3/downloads/zeros.val";
constexpr char output_file[] = "solutions.txt";

int main()
{
    acb_vector zeros(2 * N_ZEROS);
    read_zeros(zeros, path, N_ZEROS, BYTE_PRECISION);
    
    if (zeros.get_size() < N_ZEROS) 
    {
        std::cerr << "Error: Not enough zeros loaded" << std::endl;
        return 1;
    }

    const slong max_m = 2 * N_ZEROS + 1;
    solve_all(zeros, max_m, BYTE_PRECISION, output_file);

    try
    {
        auto solutions = read_solutions(output_file, BYTE_PRECISION);
        print_solutions(solutions);

        acb_t s_point, result;
        acb_init(s_point);
        acb_init(result);

        flint_printf("\nCoefficients for m=3:\n");
        for (slong i = 0; i < 3; ++i)
        {
            acb_printd(solutions.at(3).get_ptr(i), 15);
            flint_printf("\n");
        }

        const double s_values[] = {0.5, 1.0, 1.5};
        
        for (double s : s_values) 
        {
            arb_set_d(acb_realref(s_point), s);
            arb_zero(acb_imagref(s_point));
            
            compute_series(result, s_point, 3, solutions, BYTE_PRECISION);
            flint_printf("Series at s=%.1f: ", s);
            arb_printn(acb_realref(result), 15, ARB_STR_NO_RADIUS);
            flint_printf("\n");
        }

        acb_clear(s_point);
        acb_clear(result);
    }

    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}