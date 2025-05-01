#include "reader.hpp"

void read_zeros(acb_vector &zeros, const std::string &filepath, slong n_zeros,
                slong prec)
{
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return;
    }

    arb_t re, im_part;
    arb_init(re);
    arb_init(im_part);
    arb_set_str(re, "0.5", prec);

    std::string line;
    slong zeros_found = 0;

    for (slong i = 0; i < n_zeros; ++i) {
        if (!std::getline(file, line))
            break;

        if (arb_set_str(im_part, line.c_str(), prec)) {
            std::cerr << "Failed to parse: " << line << std::endl;
            continue;
        }

        // coпряженный корень
        arb_neg(im_part, im_part);
        acb_set_arb_arb(zeros[zeros_found], re, im_part);

        arb_neg(im_part, im_part);
        acb_set_arb_arb(zeros[zeros_found + 1], re, im_part);

        zeros_found += 2;
    }

    zeros.set_size(zeros_found);
    arb_clear(re);
    arb_clear(im_part);
    file.close();
}

void print_zeros(const acb_vector &zeros, slong n_zeros, slong precision)
{
    for (slong i = 0; i < std::min(zeros.get_size(), n_zeros); ++i) {
        acb_printn(zeros.get_ptr(i), precision, 0);
        flint_printf("\n");
    }
}
