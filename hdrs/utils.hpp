#ifndef UTILS_HPP
#define UTILS_HPP

#define N_PRECISION 10 // only for printing
#define BYTE_PRECISION 2048 // for calculating
#define N_ZEROS 30
#define PATH "/mnt/1DE0B2F616840AF3/downloads/zeros.val"
#define OUTPUT_FILE "solutions.txt"

struct coefficient
{
    coefficient(int idx, double re, double im)
    {
        this->idx = idx;
        this->re_value = re;
        this->im_value = im;
    };
    int idx;
    double re_value;
    double im_value;
};

#endif