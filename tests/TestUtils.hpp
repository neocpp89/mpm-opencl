#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP
#include <iostream>
#include <iomanip>
#include <string>

#include <cstdlib>

#include "Tokenizer.hpp"

#define TEST_MSG(msg, cond) do { \
 bool _test = cond; \
 std::cout << std::setw(62) << std::string(msg) << ": ";\
 if (!_test) { \
    std::cout << TerminalColors::FailedString << std::endl; \
    std::exit(EXIT_FAILURE); \
 } else { \
    std::cout << TerminalColors::PassedString << std::endl; \
 } \
} while (0)

#define TEST(cond) do { \
 TEST_MSG(#cond, cond); \
} while(0)

#define ALLPASSED(str) do { \
    auto _s = str; /* auto _s = Tokenizer::splitString(str, '/').back(); */ \
    std::cout << _s << ": " << \
    TerminalColors::Colorize("All tests passed.", TerminalColors::FG_GREEN) << std::endl; \
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
