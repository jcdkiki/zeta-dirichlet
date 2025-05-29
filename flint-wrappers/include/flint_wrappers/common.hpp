#ifndef FLINT_WRAPPERS_COMMON_HPP
#define FLINT_WRAPPERS_COMMON_HPP

#include <exception>
#include <string>

namespace flint {
class FlintException : std::exception {
    std::string str;

  public:
    FlintException(const char *str) : str(str) {}
    FlintException(const std::string &str) : str(str) {}

    const char *what() const noexcept
    {
        return str.data();
    }
};
} // namespace flint

#endif
