#ifndef ACB_WRAPPERS_COMMON_HPP
#define ACB_WRAPPERS_COMMON_HPP 

#include <string>
#include <exception>

class AcbException : std::exception {
    std::string str;
public:
    AcbException(const char *str) : str(str) {}
    AcbException(const std::string &str) : str(str) {}

    const char *what() const noexcept { return str.data(); }
};

#endif
