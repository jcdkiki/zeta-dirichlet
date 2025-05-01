#include "src/reader.hpp"
#include "src/solver.hpp"
#include "src/reader_solutions.hpp"

static constexpr int N_PRECISION = 20;
static constexpr int BYTE_PRECISION = 2048;
static constexpr int N_ZEROS = 6;
constexpr char path[] = "/home/ravendexter/Downloads/zeros.val";

int main() {
    acb_vector zeros(2 * N_ZEROS);
    read_zeros(zeros, path, N_ZEROS, BYTE_PRECISION);
    // flint_printf("Loaded %wd zeros:\n", zeros.get_size());
    // print_zeros(zeros, zeros.get_size(), 10);
    if (zeros.get_size() < N_ZEROS) {
        std::cerr << "Error: Not enough zeros loaded" << std::endl;
        return 1;
    }

    slong max_m = 2* N_ZEROS + 1;
    const char* output_file = "solutions.txt";
    solve_all(zeros, max_m, BYTE_PRECISION, output_file);

    try {
        auto solutions = read_solutions(output_file, BYTE_PRECISION);
        print_solutions(solutions);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}