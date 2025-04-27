#include "src/reader.hpp"
#include "src/solver.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 10;
const std::string path = "/mnt/1DE0B2F616840AF3/downloads/zeros.val";

int main() {
    acb_vector zeros(N_ZEROS);
    read_zeros(zeros, path, N_ZEROS, BYTE_PRECISION);

    if (zeros.get_size() < 2 * N_ZEROS) {
        std::cerr << "Error: Not enough zeros loaded" << std::endl;
        return 1;
    }

    acb_mat_t matrix;
    slong system_size = N_ZEROS + 1;

    acb_mat_init(matrix, system_size, system_size);
    make_full_matrix(matrix, system_size, zeros, BYTE_PRECISION);

    solve_all(matrix, system_size, BYTE_PRECISION);

    acb_mat_clear(matrix);
    return 0;
}