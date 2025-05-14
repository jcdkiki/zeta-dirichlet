#include "src/reader.hpp"
#include "src/solver.hpp"
#include "src/reader_solutions.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 20;
constexpr char PATH[] = "/mnt/1DE0B2F616840AF3/downloads/zeros.val";
constexpr char OUTPUT_FILE[] = "solutions.txt";

int main()
{
    acb_vector zeros(2 * N_ZEROS);
    read_zeros(zeros, PATH, N_ZEROS, BYTE_PRECISION);

    if (zeros.get_size() < N_ZEROS) 
    {
        std::cerr << "Error: Not enough zeros loaded" << std::endl;
        return 1;
    }

    const slong system_size = 2 * N_ZEROS + 1;
    solve_all(zeros, system_size, BYTE_PRECISION, OUTPUT_FILE);

    try
    {   
        auto solutions = read_solutions(OUTPUT_FILE, BYTE_PRECISION);
        
        acb_t s_point, result;
        acb_init(s_point);
        acb_init(result);

        const double s_values[] = {0.5, 1.0, 1.5, 2.0};

        int coefficients_idx = 2 * N_ZEROS + 1; 
        
        for (double s : s_values) 
        {
            arb_set_d(acb_realref(s_point), s);
            arb_zero(acb_imagref(s_point));
            
            compute_series(result, s_point, coefficients_idx, solutions, BYTE_PRECISION);
            flint_printf("Series at s=%.1f: ", s);
            arb_printn(acb_realref(result), N_PRECISION, ARB_STR_NO_RADIUS);
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