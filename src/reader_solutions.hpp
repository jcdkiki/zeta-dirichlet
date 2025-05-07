#ifndef READER_SOLUTIONS_HPP
#define READER_SOLUTIONS_HPP

#include <map>
#include <string>
#include <memory>
#include "acb_vector.hpp"

std::map<int, acb_vector> read_solutions(const char* filename, slong precision);
void print_solutions(const std::map<int, acb_vector>& solutions);

#endif