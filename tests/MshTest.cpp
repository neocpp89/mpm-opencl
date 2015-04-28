#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "catch.hpp"
#include "TestUtils.hpp"
#include "Msh.hpp"

std::string simple_mshfile_contents =
R"(
$MeshFormat
2.2 0 8
$EndMeshFormat
$PhysicalNames
1
2 100 "CMesh"
$EndPhysicalNames
$Nodes
4
1 0 0 0
2 1 0 0
3 1 1 0
4 0 1 0
$EndNodes
$Elements
6
1 1 2 99 1 1 2
2 1 2 99 1 2 3
3 1 2 99 1 3 4
4 1 2 99 1 4 1
5 2 2 100 2 1 2 3
6 2 2 100 2 1 3 4
$EndElements
)";

TEST_CASE("Read mshfile", "[msh][short]")
{
    std::istringstream iss(simple_mshfile_contents);
    msh::Mesh exampleMsh(iss);
}

TEST_CASE("Write MT2 file", "[msh][short]")
{
    std::istringstream iss(simple_mshfile_contents);
    msh::Mesh exampleMsh(iss);
    std::ostringstream oss;
    exampleMsh.writeMT2(oss);
}

