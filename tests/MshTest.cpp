#include <iostream>
#include <fstream>

#include "TestUtils.hpp"
#include "Msh.hpp"

int main(int argc, char **argv)
{
    std::ifstream f("example.msh");
    msh::Mesh exampleMsh(f);
    ALLPASSED(argv[0]);
    return 0;
}

