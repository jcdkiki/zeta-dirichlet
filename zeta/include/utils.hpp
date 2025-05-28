#ifndef UTILS_HPP
#define UTILS_HPP

struct coefficient {
    coefficient(int idx, double real, double imag) : idx(idx), real(real), imag(imag) {}
    
    int    idx;
    double real;
    double imag;
};

#endif
