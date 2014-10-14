#include <iostream>

#include "TestUtils.hpp"
#include "Tensor.hpp"

int main(int argc, char **argv)
{
    Tensor<double, 3> S({1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0});

    //Output tensor
    std::cout << "S = " << S << std::endl;

    // Test dimensions
    TEST(S.dim() == 3);

    // Test initialization
    bool initializationTest = true;
    for (size_t i = 0; i < S.dim(); i++) {
        for (size_t j = 0; j < S.dim(); j++) {
            if(S(i, j) != 1.0 + i * S.dim() + j) {
                initializationTest = false;
            }
        }
    }
    TEST(initializationTest);

    // Test copy constructor
    Tensor<double, 3> A(S);
    bool copyConstructorTest = true;
    for (size_t i = 0; i < A.dim(); i++) {
        for (size_t j = 0; j < A.dim(); j++) {
            if(S(i, j) != A(i, j)) {
                copyConstructorTest = false;
            }
        }
    }
    std::cout << "A = " << A << std::endl;
    TEST(copyConstructorTest);
    
    // Test equality operator
    TEST(S == A);
    Tensor<double, 3> B(A);
    B(0,0) = 2;
    std::cout << "B = " << B << std::endl;
    TEST(S != B);

    // Test trace
    TEST(S.trace() == 15.0);
    TEST(B.trace() == 16.0);

    // Test deviator
    Tensor<double, 3> expectedA0({-4,2,3,4,0,6,7,8,4});
    Tensor<double, 3> A0;
    double sphA;
    A.decompose(A0, sphA);
    std::cout << "A0 = " << A0 << std::endl;
    TEST(A.trace() == A.dim() * sphA);
    TEST(A0 == expectedA0);

    // Test contraction
    TEST(A0.contraction(A) == 210.0);
    TEST(A0.contraction(B) == 206.0);

    // Test operators
    double c = 2;
    Tensor<double, 3> expectedMult({2.0,4.0,6.0,8.0,10.0,12.0,14.0,16.0,18.0});
    TEST(c * A == expectedMult);
    TEST(A * c == expectedMult);
    Tensor<double, 3> expectedAdd({-3.0,4.0,6.0,8.0,5.0,12.0,14.0,16.0,13.0});
    TEST(A + expectedMult == 3 * A);
    TEST(A + A0 != 3 * A);
    TEST(A + A0 == expectedAdd);

    ALLPASSED(argv[0]);
    return 0;
}
