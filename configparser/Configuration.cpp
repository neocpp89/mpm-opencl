#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional> 
#include <cctype>
#include <locale>

#include "Tokenizer.hpp"
#include "Configuration.hpp"
/*
    Trimming code from:
    http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
*/
// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

// Static Configuration class implementation
void StaticConfiguration::binOptions()
{
    /* check each key and put in appropriate typed dictionary */
    for (auto const & kv : dict) {
        setInTypedDict(kv.first, kv.second);
    }
}

void StaticConfiguration::setInTypedDict(std::string const &k, std::string const &v)
{
    if (std::find(doubleOptions.begin(), doubleOptions.end(), k) != doubleOptions.end()) {
        // this is a double option
        doubleDict[k] = std::stod(v);
    } else if (std::find(intOptions.begin(), intOptions.end(), k) != intOptions.end()) {
        // this is an int option
        intDict[k] = std::stoi(v);
    } else if (std::find(sizeTOptions.begin(), sizeTOptions.end(), k) != sizeTOptions.end()) {
        // this is a size_t option
        sizeTDict[k] = std::stoull(v);
    } else if (std::find(stringOptions.begin(), stringOptions.end(), k) != stringOptions.end()) {
        // this is a string option
        stringDict[k] = v;
    } else {
        // we don't know what this is
        if (strict) {
            std::cerr << "FATAL: unknown key-value pair [" << k << ", " << v << "]" << std::endl;
            exit(1);
        } else {
            std::cerr << "Ignoring unknown key-value pair: [" << k << ", " << v << "]" << std::endl;
        }
    }
    return;
}

void StaticConfiguration::readConfigStream(std::istream &cfgstream)
{
    std::size_t line = 0;
    do {
        line++;
        std::vector<std::string> tokens = Tokenizer::splitNextLine(cfgstream, '=');
        if (cfgstream.eof() || !cfgstream.good()) {
            break;
        }
        if (tokens.size() == 0 || tokens[0][0] == '#') {
            // comment or blank line; ignore it
            continue;
        }
        if (tokens.size() == 2) {
            dict[trim(tokens[0])] = ltrim(tokens[1]);
        } else {
            if (strict) {
                std::cerr << "FATAL: line " << line;
                std::cerr << " has incorrect number of tokens (";
                std::cerr << tokens.size() << ") [";
                for (auto const &t : tokens) {
                    std::cerr << " " << t;
                }
                std::cerr<< " ]" << std::endl;
                exit(1);
            } else {
                std::cerr << "Ignoring line with " << tokens.size() << " tokens." << std::endl;
            }
        }
    } while (true);
    updateConfig();
    return;
}

void StaticConfiguration::readConfigFile(std::string const &cfgfile)
{
    std::ifstream cfgstream(cfgfile);
    this->readConfigStream(cfgstream);
    return;
}
void StaticConfiguration::addDoubleOption(std::string s, double default_value)
{
    doubleOptions.push_back(trim(s));
    doubleDict[s] = default_value;
    return;
}

void StaticConfiguration::addIntOption(std::string s, int default_value)
{
    intOptions.push_back(trim(s));
    intDict[s] = default_value;
    return;
}

void StaticConfiguration::addSizeTOption(std::string s, size_t default_value)
{
    sizeTOptions.push_back(trim(s));
    sizeTDict[s] = default_value;
    return;
}

void StaticConfiguration::addStringOption(std::string s, std::string const & default_value)
{
    stringOptions.push_back(trim(s));
    stringDict[s] = default_value;
    return;
}


bool StaticConfiguration::keyExists(std::string const &k) const
{
    return ((dict.count(k) != 0)
            || (intDict.count(k) != 0)
            || (doubleDict.count(k) != 0)
            || (sizeTDict.count(k) != 0)
            || (stringDict.count(k) != 0));
}

double StaticConfiguration::getDoubleOption(std::string const &k) const
{
    double r = doubleDict.at(k);
    return r;
}

int StaticConfiguration::getIntOption(std::string const &k) const
{
    int r = intDict.at(k);
    return r;
}

size_t StaticConfiguration::getSizeTOption(std::string const &k) const
{
    size_t r = sizeTDict.at(k);
    return r;
}

std::string StaticConfiguration::getStringOption(std::string const &k) const
{
    std::string const &r = stringDict.at(k);
    return r;
}


void StaticConfiguration::setDoubleOption(std::string const &k, double v)
{
    doubleDict[k] = v;
    return;
}

void StaticConfiguration::setIntOption(std::string const &k, int v)
{
    intDict[k] = v;
    return;
}

void StaticConfiguration::setSizeTOption(std::string const &k, size_t v)
{
    sizeTDict[k] = v;
    return;
}

void StaticConfiguration::setStringOption(std::string const &k, std::string const &v)
{
    stringDict[k] = v;
    return;
}

void StaticConfiguration::setTypedOption(std::string const &k, std::string const &v)
{
    setInTypedDict(k, v);
    return;
}

std::ostream& operator<<(std::ostream& os, const StaticConfiguration& sc)
{
    os << "[\n    doubles = [\n";
    for (auto const & kv : sc.doubleDict) {
        os << "        [" << kv.first << " = " << kv.second << "]\n";
    }
    os << "    ],\n    ints = [\n";
    for (auto const & kv : sc.intDict) {
        os << "        [" << kv.first << " = " << kv.second << "]\n";
    }
    os << "    ],\n    size_ts = [\n";
    for (auto const & kv : sc.sizeTDict) {
        os << "        [" << kv.first << " = " << kv.second << "]\n";
    }
    os << "    ],\n    strings = [\n";
    for (auto const & kv : sc.stringDict) {
        os << "        [" << kv.first << " = " << kv.second << "]\n";
    }
    os << "    ]\n]";
    return os;
}

