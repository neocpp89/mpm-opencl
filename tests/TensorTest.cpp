#include <iostream>

#include "TestUtils.hpp"
#include "Tensor.hpp"

int main(int argc, char **argv)
{
    Tensor<int, 3> S;

    TEST(S.dim() == 3);
    
    for (int s = 0; s < 3; s++) {
        for (int x = 0; x < 3; x++) {
            S(s,x) = s+x;
        }
    }

    TEST(S(0,0) == 2);


    for (int s = 0; s < 3; s++) {
        for (int x = 0; x < 3; x++) {
            std::cout << S(s,x) << std::endl;
        }
    }
    
    std::cout << TerminalColors::PassedString << std::endl;
    std::cout << "trololol" << std::endl;

    return 0;
}
