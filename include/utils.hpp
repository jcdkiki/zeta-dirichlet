#ifndef UTILS_HPP
#define UTILS_HPP

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
