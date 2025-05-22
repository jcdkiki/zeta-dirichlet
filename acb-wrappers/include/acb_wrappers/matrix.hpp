#ifndef ACB_WRAPPERS_MATRIX_HPP
#define ACB_WRAPPERS_MATRIX_HPP

#include <flint/acb.h>
#include <flint/acb_mat.h>
#include "acb_wrappers/vector.hpp"

class acb_matrix 
{
private:
    acb_mat_t mat;
    slong rows;
    slong cols;

public:
    acb_matrix(const acb_matrix&) = delete;
    acb_matrix& operator=(const acb_matrix&) = delete;

    acb_matrix(slong r, slong c);
    acb_matrix(acb::Vector& vec);
    acb_matrix(acb_matrix&& other) noexcept;
    acb_matrix& operator=(acb_matrix&& other) noexcept;

    ~acb_matrix();

    acb_mat_t* get_mat() noexcept { return &mat; }
    const acb_mat_t* get_mat() const noexcept { return &mat; };

    slong row_count() const noexcept { return rows; }
    slong col_count() const noexcept { return cols; }

    acb_ptr operator()(slong row);
    const acb_ptr operator()(slong row) const;
};

void print_matrix(acb_matrix matrix, slong matrix_size);

acb::Vector matrix_vector_multiply(const acb_matrix &M, const acb::Vector &v);

#endif
