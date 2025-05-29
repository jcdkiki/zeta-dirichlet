#ifndef COMMON_HPP
#define COMMON_HPP

#include <exception>
#include <string>

class ZetaException : std::exception {
    std::string str;

  public:
    ZetaException(const char *str) : str(str) {}
    ZetaException(const std::string &str) : str(str) {}

    const char *what() const noexcept
    {
        return str.data();
    }
};

#endif
