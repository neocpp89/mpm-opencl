#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <fstream>
#include <string>
#include <vector>

class Tokenizer
{
    public:
        Tokenizer() { return; }
        static std::vector<std::string>
            splitNextLine(std::istream& str, const char delim = ' ');
        static std::vector<std::string>
            splitString(std::string& str, const char delim = ' ');
        static std::vector<std::string>
            splitString(const char *str, const char delim = ' ');
};

#endif //TOKENIZER_HPP
