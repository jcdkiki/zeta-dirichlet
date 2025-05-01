#include "src/reader.hpp"
#include "src/solver.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 6;
constexpr char path[] = "/home/ravendexter/Downloads/zeros.val";

int main() {
    acb_vector zeros(N_ZEROS);
    read_zeros(zeros, path, N_ZEROS, BYTE_PRECISION);
    // flint_printf("Loaded %wd zeros:\n", zeros.get_size());
    // print_zeros(zeros, zeros.get_size(), 10);
    if (zeros.get_size() < N_ZEROS) {
        std::cerr << "Error: Not enough zeros loaded" << std::endl;
        return 1;
    }

    slong max_m = 2* N_ZEROS + 1;
    solve_all(zeros, max_m, BYTE_PRECISION);

    return 0;
}