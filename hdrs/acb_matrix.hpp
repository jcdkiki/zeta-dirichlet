#ifndef ACB_MATRIX_HPP
#define ACB_MATRIX_HPP

#include <flint/acb.h>
#include <flint/acb_mat.h>
#include <stdexcept>

class acb_matrix {
private:
    acb_mat_t mat;
    slong rows;
    slong cols;

public:
    acb_matrix(const acb_matrix&) = delete;
    acb_matrix& operator=(const acb_matrix&) = delete;

    acb_matrix(slong r, slong c);
    acb_matrix(acb_matrix&& other) noexcept;
    acb_matrix& operator=(acb_matrix&& other) noexcept;

    ~acb_matrix();

    acb_mat_t* get_mat() noexcept;
    const acb_mat_t* get_mat() const noexcept;

    slong row_count() const noexcept;
    slong col_count() const noexcept;

    acb_ptr operator()(slong row);
    const acb_ptr operator()(slong row) const;
};

void print_matrix(acb_matrix matrix, slong matrix_size);

#endif
