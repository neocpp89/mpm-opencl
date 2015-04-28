#include "catch.hpp"
#include "Tensor.hpp"

TEST_CASE("Tensor Sizes", "[tensor][short]")
{
    Tensor<double, 3> A({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 2> B({1.0,2.0,3.0,4.0});
    REQUIRE(A.dim() == 3);
    REQUIRE(B.dim() == 2);
}

TEST_CASE("Tensor Equality", "[tensor][short]")
{
    Tensor<double, 3> A({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> B({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> C({10.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    REQUIRE(A == A);
    REQUIRE(B == B);
    REQUIRE(A == B);
    REQUIRE(A != C);
    REQUIRE(B != C);
}

TEST_CASE("Tensor Initialization", "[tensor][short]")
{
    Tensor<double, 3> S({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    for (size_t i = 0; i < S.dim(); i++) {
        for (size_t j = 0; j < S.dim(); j++) {
            REQUIRE(S(i, j) == (1.0 + i * S.dim() + j));
        }
    }
}

TEST_CASE("Tensor Copy Constructor", "[tensor][short]")
{
    Tensor<double, 3> S({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> A(S);
    REQUIRE(A == S);
    for (size_t i = 0; i < A.dim(); i++) {
        for (size_t j = 0; j < A.dim(); j++) {
            REQUIRE(S(i, j) == A(i, j));
        }
    }
}

TEST_CASE("Tensor Trace", "[tensor][short]")
{
    Tensor<double, 3> S({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> B({2.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    REQUIRE(S.trace() == 15.0);
    REQUIRE(B.trace() == 16.0);
}

TEST_CASE("Tensor Deviator", "[tensor][short]")
{
    Tensor<double, 3> A({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> expectedA0({-4,2,3,4,0,6,7,8,4});
    double sphA;
    Tensor<double, 3> A0;
    A.decompose(A0, sphA);
    REQUIRE(A0 == expectedA0);
    REQUIRE(A.trace() == (sphA * A.dim()));
}

TEST_CASE("Tensor Contraction", "[tensor][short]")
{
    Tensor<double, 3> A({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> A0({-4,2,3,4,0,6,7,8,4});
    Tensor<double, 3> B({2.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    REQUIRE(A0.contraction(A) == 210.0);
    REQUIRE(A0.contraction(B) == 206.0);
}

TEST_CASE("Tensor Operations", "[tensor][short]")
{
    Tensor<double, 3> A({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});
    Tensor<double, 3> A0({-4,2,3,4,0,6,7,8,4});
    // Test operators
    double c = 2;
    Tensor<double, 3> expectedMult({2.0,4.0,6.0,8.0,10.0,12.0,14.0,16.0,18.0});
    REQUIRE((c * A) == expectedMult);
    REQUIRE((A * c) == expectedMult);
    Tensor<double, 3> expectedAdd({-3.0,4.0,6.0,8.0,5.0,12.0,14.0,16.0,13.0});
    REQUIRE((A + expectedMult) == (3 * A));
    REQUIRE((A + A0) != (3 * A));
    REQUIRE((A + A0) == expectedAdd);
}
