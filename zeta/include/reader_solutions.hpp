#ifndef READER_SOLUTIONS_HPP
#define READER_SOLUTIONS_HPP

#include <map>
#include <acb_wrappers/vector.hpp>

std::map<int, acb::Vector> read_solutions(const char *filename, slong precision);
void print_solutions(const std::map<int, acb::Vector> &solutions);

#endif
