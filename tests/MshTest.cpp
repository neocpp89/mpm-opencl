#include <iostream>
#include <fstream>

#include "TestUtils.hpp"
#include "Msh.hpp"

int main(int argc, char **argv)
{
    std::ifstream f("example.msh");
    msh::Mesh exampleMsh(f);
    std::ofstream o("example.mt2");
    exampleMsh.writeMT2(o);
    ALLPASSED(argv[0]);
    return 0;
}

