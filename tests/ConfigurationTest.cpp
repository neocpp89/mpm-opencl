#include <iostream>

#include "TestUtils.hpp"
#include "Configuration.hpp"

int main(int argc, char **argv)
{
    StaticConfiguration scfg;
    Configuration *cfg = dynamic_cast<Configuration *>(&scfg);

    // set up one of each type of option
    scfg.addDoubleOption("double_option", 0.25);
    scfg.addIntOption("int_option", 36);
    scfg.addStringOption("string_option", "Hello, World!");
    scfg.addSizeTOption("sizet_option", 203L);
    TEST(cfg->getDoubleOption("double_option") == 0.25);
    TEST(cfg->getIntOption("int_option") == 36);
    TEST(cfg->getStringOption("string_option") == "Hello, World!");
    TEST(cfg->getSizeTOption("sizet_option") == 203L);

    // try to change each of them
    scfg.setDoubleOption("double_option", 0.5);
    scfg.setIntOption("int_option", -8);
    scfg.setStringOption("string_option", "bar");
    scfg.setSizeTOption("sizet_option", 9L);
    TEST(cfg->getDoubleOption("double_option") == 0.5);
    TEST(cfg->getIntOption("int_option") == -8);
    TEST(cfg->getStringOption("string_option") == "bar");
    TEST(cfg->getSizeTOption("sizet_option") == 9L);

    // actually parse a configuration file specified to override defaults
    const std::string cfgfile = "test.cfg";
    //std::cout << "Read configuration file (\"" << cfgfile << "\")." << std::endl;
    scfg.readConfigFile(cfgfile);
    // std::cout << "Configuration: " << scfg << std::endl;
    TEST(cfg->getDoubleOption("double_option") == 45.25);
    TEST(cfg->getIntOption("int_option") == 32);
    TEST(cfg->getStringOption("string_option") == "foo bar");
    TEST(cfg->getSizeTOption("sizet_option") == 352L);

    // try to read a nonexistent key
    bool nonexistentKeyTest = false;
    try {
        const double nonexistent = cfg->getDoubleOption("minRadius");
        std::cout << "Dubious key value (should not reach this!): "
            << nonexistent << '\n';
        nonexistentKeyTest = false;
    } catch (std::out_of_range &e) {
        nonexistentKeyTest = true;
    }
    TEST(nonexistentKeyTest);

    ALLPASSED(argv[0]);
    return 0;
}

