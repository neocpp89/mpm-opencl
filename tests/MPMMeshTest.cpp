#include "catch.hpp"
#include "Mesh.hpp"

TEST_CASE("Structured Fixed Aspect 2D Mesh Initialization", "[SAR2][short]")
{
    Point2<double> ul = {0, 1};
    Point2<double> lr = {1, 0};

    StructuredFixedAR2D<double> mpmmesh(ul, lr, 10, 10);
    REQUIRE(mpmmesh.getNumberOfElements() == 100);
}

