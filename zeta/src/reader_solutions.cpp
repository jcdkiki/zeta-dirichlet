#include "reader_solutions.hpp"
#include <acb_wrappers/vector.hpp>
#include <fstream>
#include <regex>
#include <vector>
#include <memory>
#include <stdexcept>
#include <flint/acb.h>

using namespace std;

std::map<int, acb::Vector> read_solutions(const char *path, slong precision) 
{
    map<int, acb::Vector> solutions;
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + string(path));
    }

    string line;
    regex m_pattern(R"(Solution for m=(\d+):)");
    regex entry_pattern(R"(\s*\(\s*([^\+-]+)\s*([+-])\s*([^j]+)j\s*\)\s*\+\/-\s*\([^\)]+\))");

    int current_m = -1;
    vector<unique_ptr<acb_struct[]>> current_coeffs;

    while (getline(file, line)) 
    {
        smatch match;
        if (regex_search(line, match, m_pattern)) 
        {
            if (current_m != -1) 
            {
                acb::Vector vec(current_coeffs.size());
                for (size_t i = 0; i < current_coeffs.size(); ++i) 
                {
                    acb_set(vec[i], current_coeffs[i].get());
                }
                solutions.emplace(current_m, std::move(vec));
                current_coeffs.clear();
            }
            current_m = stoi(match[1]);
        }
        else if (current_m != -1 && regex_search(line, match, entry_pattern)) 
        {
            string real_str = match[1].str();
            string sign = match[2].str();
            string imag_str = sign + match[3].str();

            auto coeff = make_unique<acb_struct[]>(1);
            acb_init(coeff.get());

            if (arb_set_str(acb_realref(coeff.get()), real_str.c_str(), precision) != 0 ||
                arb_set_str(acb_imagref(coeff.get()), imag_str.c_str(), precision) != 0) 
            {
                throw runtime_error("Failed to parse coefficient: " + real_str + " " + imag_str + "j");
            }

            current_coeffs.push_back(std::move(coeff));
        }
    }

    if (current_m != -1 && !current_coeffs.empty()) 
    {
        acb::Vector vec(current_coeffs.size());
        for (size_t i = 0; i < current_coeffs.size(); ++i) 
        {
            acb_set(vec[i], current_coeffs[i].get());
        }
        solutions.emplace(current_m, std::move(vec));
    }

    return solutions;
}

void print_solutions(const map<int, acb::Vector> &solutions) 
{
    for (const auto& [m, vec] : solutions) 
    {
        flint_printf("Solution for m=%d:\n", m);
        for (slong i = 0; i < vec.size(); ++i) 
        {
            acb_printd(vec[i], 15);
            flint_printf("\n");
        }
        flint_printf("\n");
    }
}
