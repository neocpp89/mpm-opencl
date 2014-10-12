#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP
#include <iostream>
#include <iomanip>
#include <string>

#define TEST(cond) do { \
 bool _test = cond; \
 std::cout << std::setw(42) << std::string(#cond) << ": ";\
 if (!_test) { \
    std::cout << TerminalColors::FailedString << std::endl; exit(1); \
 } else { \
    std::cout << TerminalColors::PassedString << std::endl; \
 } \
} while (0)

namespace TerminalColors {
    enum Codes {
        FG_RED = 31,
        FG_GREEN = 32
    };

    std::string Colorize(std::string const &s, int code = 39)
    {
        return "\033[" + std::to_string(code) + "m" + s + "\033[39m";
    }
    
    const std::string FailedString = Colorize("FAILED", FG_RED);
    const std::string PassedString = Colorize("PASSED", FG_GREEN);
}
#endif //TEST_UTILS_HPP
