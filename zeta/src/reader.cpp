#include "reader.hpp"
#include <flint_wrappers/real.hpp>
#include <fstream>
#include <iostream>

void read_zeros(flint::Vector &zeros, const char *filepath, slong n_zeros, slong prec)
{
    // TODO: throw exception on failure (not enough zeros or failed to open file)

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return;
    }

    flint::Real real = 0.5;
    flint::Real imag;
    
    std::string line;
    slong zeros_found = 0;

    for (slong i = 0; i < n_zeros; ++i) {
        if (!std::getline(file, line))
            break;

        if (imag.set(line.c_str(), prec)) {
            std::cerr << "Failed to parse: " << line << std::endl;
            continue;
        }

        zeros[zeros_found++].set(real, imag);
        
        // coпряженный корень
        neg(imag, imag);
        zeros[zeros_found++].set(real, imag);
    }
}

void print_zeros(const flint::Vector &zeros, slong n_zeros, slong precision)
{
    for (slong i = 0; i < std::min(zeros.size(), n_zeros); ++i) {
        acb_printn(zeros[i].get(), precision, 0);
        flint_printf("\n");
    }
}
