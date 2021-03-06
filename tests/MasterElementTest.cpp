#include "catch.hpp"

#include <vector>
#include "MasterElement.hpp"

TEST_CASE("P1 Quad Shape Functions", "[master-element][short]")
{
    master_element::Point2<double> n0 = {-1, -1};
    master_element::Point2<double> n1 = {1, -1};
    master_element::Point2<double> n2 = {1, 1};
    master_element::Point2<double> n3 = {-1, 1};
    master_element::Point2<double> mid = {0, 0};
    master_element::Point2<double> midhr = {1, 0};
    master_element::Point2<double> midhl = {-1, 0};
    master_element::Point2<double> midvt = {0, 1};
    master_element::Point2<double> midvb = {0, -1};

    master_element::P1Quad<double> master;

    std::vector<double> result(4, 0);

    master.phi(n0, result.begin(), result.end());
    REQUIRE(result[0] == 1);
    REQUIRE(result[1] == 0);
    REQUIRE(result[2] == 0);
    REQUIRE(result[3] == 0);

    master.phi(n1, result.begin(), result.end());
    REQUIRE(result[0] == 0);
    REQUIRE(result[1] == 1);
    REQUIRE(result[2] == 0);
    REQUIRE(result[3] == 0);

    master.phi(n2, result.begin(), result.end());
    REQUIRE(result[0] == 0);
    REQUIRE(result[1] == 0);
    REQUIRE(result[2] == 1);
    REQUIRE(result[3] == 0);

    master.phi(n3, result.begin(), result.end());
    REQUIRE(result[0] == 0);
    REQUIRE(result[1] == 0);
    REQUIRE(result[2] == 0);
    REQUIRE(result[3] == 1);

    master.phi(mid, result.begin(), result.end());
    REQUIRE(result[0] == 0.25);
    REQUIRE(result[1] == 0.25);
    REQUIRE(result[2] == 0.25);
    REQUIRE(result[3] == 0.25);

    master.phi(midhr, result.begin(), result.end());
    REQUIRE(result[0] == 0);
    REQUIRE(result[1] == 0.5);
    REQUIRE(result[2] == 0.5);
    REQUIRE(result[3] == 0);

    master.phi(midhl, result.begin(), result.end());
    REQUIRE(result[0] == 0.5);
    REQUIRE(result[1] == 0);
    REQUIRE(result[2] == 0);
    REQUIRE(result[3] == 0.5);

    master.phi(midvt, result.begin(), result.end());
    REQUIRE(result[0] == 0);
    REQUIRE(result[1] == 0);
    REQUIRE(result[2] == 0.5);
    REQUIRE(result[3] == 0.5);

    master.phi(midvb, result.begin(), result.end());
    REQUIRE(result[0] == 0.5);
    REQUIRE(result[1] == 0.5);
    REQUIRE(result[2] == 0);
    REQUIRE(result[3] == 0);
}

TEST_CASE("P1 Quad Gradient of Shape Functions", "[master-element][short]")
{
    master_element::Point2<double> n0 = {-1, -1};
    master_element::Point2<double> n1 = {1, -1};
    master_element::Point2<double> n2 = {1, 1};
    master_element::Point2<double> n3 = {-1, 1};
    master_element::Point2<double> mid = {0, 0};
    master_element::Point2<double> midhr = {1, 0};
    master_element::Point2<double> midhl = {-1, 0};
    master_element::Point2<double> midvt = {0, 1};
    master_element::Point2<double> midvb = {0, -1};

    master_element::P1Quad<double> master;

    std::vector<master_element::Point2<double>> result(4, {0, 0});
    master_element::Point2<double> expected_g0 = {-0.5, -0.5};
    master_element::Point2<double> expected_g1 = {0.5, 0};
    master_element::Point2<double> expected_g2 = {0, 0};
    master_element::Point2<double> expected_g3 = {0, 0.5};

    master.gradphi(n0, result.begin(), result.end());
    REQUIRE(result[0] == expected_g0);
    REQUIRE(result[1] == expected_g1);
    REQUIRE(result[2] == expected_g2);
    REQUIRE(result[3] == expected_g3);
}
