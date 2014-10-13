#include <iostream>

#include "TestUtils.hpp"
#include "Configuration.hpp"

int main(int argc, char **argv)
{

    /*
        Create configuration file parsing class and expected key-value pairs.
        Also includes default values for the configuration options.
    */
    StaticConfiguration scfg;
    Configuration *cfg = dynamic_cast<Configuration *>(&scfg);

    scfg.addDoubleOption("double_option", 0.25);
    scfg.addIntOption("int_option", 36);
    scfg.addStringOption("string_option", "Hello, World!");
    scfg.addSizeTOption("sizet_option", 203L);

    TEST(cfg->getDoubleOption("double_option") == 0.25);
    TEST(cfg->getIntOption("int_option") == 36);
    TEST(cfg->getStringOption("string_option") == "Hello, World!");
    TEST(cfg->getSizeTOption("sizet_option") == 203L);

    std::string cfgfile = "default.cfg";

    /*
        Parse command line arguments.
    */
    if (argc % 2 != 1) {
        exit(1);
    } else {
        std::vector<std::string> keys;
        std::vector<std::string> vals;
        for (int i = 1; i < argc; i += 2) {
            keys.push_back(argv[i]);
            // We know this access is safe because each argument is paired.
            vals.push_back(argv[i+1]);
        }

        // Check if we're asking for help
        auto helpit = std::find(keys.begin(), keys.end(), "--help");
        if (helpit != keys.end()) {
            exit(0);
        }

        // Check if we specified the config file on the command line.
        auto cfgit = std::find(keys.begin(), keys.end(), "cfgfile");
        if (cfgit != keys.end()) {
            size_t idx = cfgit - keys.begin();
            cfgfile = vals[idx];

            // remove config file settings from options
            keys.erase(cfgit);
            vals.erase(idx + vals.begin());
        }

        // actually parse the configuration file specified to override defaults
        std::cout << "Read configuration file (\"" << cfgfile << "\")." << std::endl;
        scfg.readConfigFile(cfgfile);

        // overwrite configuration options with those on the command line.
        for (size_t i = 0; i < keys.size(); i++) {
            if (scfg.keyExists(keys[i])) {
                scfg.setTypedOption(keys[i], vals[i]);
            } else {
                std::cout << "Unknown option: " << keys[i] << "." << std::endl;
                exit(1);
            }
        }
    }

    try {
        const double nonexistent = cfg->getDoubleOption("minRadius");
    } catch (std::out_of_range &e) {
        TEST(e.what());
    }

    return 0;
}

