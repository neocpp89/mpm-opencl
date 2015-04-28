#include <iostream>
#include <string>
#include <sstream>

#include "catch.hpp"
#include "Configuration.hpp"

std::string cfgfile_contents =
R"(
double_option = 45.25
int_option = 32
string_option = foo bar
sizet_option = 352L
)";

TEST_CASE("Static Configuration Initialization", "[configuration][short]")
{
    StaticConfiguration scfg;
    Configuration *cfg = dynamic_cast<Configuration *>(&scfg);

    // set up one of each type of option
    scfg.addDoubleOption("double_option", 0.25);
    scfg.addIntOption("int_option", 36);
    scfg.addStringOption("string_option", "Hello, World!");
    scfg.addSizeTOption("sizet_option", 203L);
    REQUIRE(cfg->getDoubleOption("double_option") == 0.25);
    REQUIRE(cfg->getIntOption("int_option") == 36);
    REQUIRE(cfg->getStringOption("string_option") == "Hello, World!");
    REQUIRE(cfg->getSizeTOption("sizet_option") == 203L);
}

TEST_CASE("Static Configuration Modification", "[configuration][short]")
{
    StaticConfiguration scfg;
    Configuration *cfg = dynamic_cast<Configuration *>(&scfg);

    // set up one of each type of option
    scfg.addDoubleOption("double_option", 0.25);
    scfg.addIntOption("int_option", 36);
    scfg.addStringOption("string_option", "Hello, World!");
    scfg.addSizeTOption("sizet_option", 203L);

    // try to change each of them
    scfg.setDoubleOption("double_option", 0.5);
    scfg.setIntOption("int_option", -8);
    scfg.setStringOption("string_option", "bar");
    scfg.setSizeTOption("sizet_option", 9L);
    REQUIRE(cfg->getDoubleOption("double_option") == 0.5);
    REQUIRE(cfg->getIntOption("int_option") == -8);
    REQUIRE(cfg->getStringOption("string_option") == "bar");
    REQUIRE(cfg->getSizeTOption("sizet_option") == 9L);
}

TEST_CASE("Static Configuration Stream Read", "[configuration][short]")
{
    StaticConfiguration scfg;
    Configuration *cfg = dynamic_cast<Configuration *>(&scfg);

    // set up one of each type of option
    scfg.addDoubleOption("double_option", 0.25);
    scfg.addIntOption("int_option", 36);
    scfg.addStringOption("string_option", "Hello, World!");
    scfg.addSizeTOption("sizet_option", 203L);

    // actually parse a configuration file specified to override defaults
    std::istringstream iss(cfgfile_contents);
    //std::cout << "Read configuration file (\"" << cfgfile << "\")." << std::endl;
    scfg.readConfigStream(iss);
    // std::cout << "Configuration: " << scfg << std::endl;
    REQUIRE(cfg->getDoubleOption("double_option") == 45.25);
    REQUIRE(cfg->getIntOption("int_option") == 32);
    REQUIRE(cfg->getStringOption("string_option") == "foo bar");
    REQUIRE(cfg->getSizeTOption("sizet_option") == 352L);

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
    REQUIRE(nonexistentKeyTest == true);
}

