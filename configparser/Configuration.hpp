#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional> 
#include <cctype>
#include <locale>

#include "Tokenizer.hpp"
class Configuration
{
    public:
        virtual ~Configuration() { return; }
        virtual bool keyExists(std::string const &) const = 0;

        virtual double getDoubleOption(std::string const &) const = 0;
        virtual int getIntOption(std::string const &) const = 0;
        virtual size_t getSizeTOption(std::string const &) const = 0;
        virtual std::string getStringOption(std::string const &) const = 0;

        virtual void setDoubleOption(std::string const &, double) = 0;
        virtual void setIntOption(std::string const &, int) = 0;
        virtual void setSizeTOption(std::string const &, size_t) = 0;
        virtual void setStringOption(std::string const &, std::string const &) = 0;
};

class StaticConfiguration : public Configuration
{
    std::ifstream cfgstrm;
    bool strict;

    private:
        std::unordered_map<std::string, std::string> dict; // key-value pairs

        std::vector<std::string> doubleOptions;
        std::unordered_map<std::string, double> doubleDict;

        std::vector<std::string> intOptions;
        std::unordered_map<std::string, int> intDict;

        std::vector<std::string> sizeTOptions;
        std::unordered_map<std::string, size_t> sizeTDict;

        std::vector<std::string> stringOptions;
        std::unordered_map<std::string, std::string> stringDict;

        void binOptions();
        void setInTypedDict(std::string const &k, std::string const &v);

    public:
        StaticConfiguration(bool _strict = true) : strict(_strict) { return; }
        ~StaticConfiguration() { return; }

        void readConfigFile(std::string const &cfgfile);
        void updateConfig() { binOptions(); return; }
        
        void addDoubleOption(std::string s, double default_value = 0);
        void addIntOption(std::string s, int default_value = 0);
        void addSizeTOption(std::string s, size_t default_value = 0);
        void addStringOption(std::string s, std::string const & default_value = "");

        bool keyExists(std::string const &k) const;

        double getDoubleOption(std::string const &k) const;
        int getIntOption(std::string const &k) const;
        size_t getSizeTOption(std::string const &k) const;
        std::string getStringOption(std::string const &k) const;
        
        void setDoubleOption(std::string const &k, double v);
        void setIntOption(std::string const &k, int v);
        void setSizeTOption(std::string const &k, size_t v);
        void setStringOption(std::string const &k, std::string const &v);

        void setTypedOption(std::string const &k, std::string const &v);

        friend std::ostream& operator<<(std::ostream& os, const StaticConfiguration& sc);
};

#endif //CONFIGURATION_HPP

