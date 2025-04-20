#include "src/reader.hpp"
#include "src/solver.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 100;

int main()
{
    acb_vector zeros(N_ZEROS);

    read_zeros(zeros, path, N_ZEROS, BYTE_PRECISION);
    
    // print_zeros(zeros, N_ZEROS, N_PRECISION);

    acb_mat_t matrix;

    slong system_size = 2 * N_ZEROS + 1;

    acb_mat_init(matrix, system_size, system_size);

    make_matrix(matrix, system_size, zeros, BYTE_PRECISION);

    solve(matrix, system_size, BYTE_PRECISION);

    acb_mat_clear(matrix);

    return 0;
}