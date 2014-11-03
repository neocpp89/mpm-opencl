#include <iostream>
#include <iterator>

#include "TestUtils.hpp"
#include "Graph.hpp"

int main(int argc, char **argv)
{
    UndirectedGraph<int> g;
    g.connect(0, 10);
    g.connect(0, 1);
    g.connect(1, 2);
    g.connect(2, 10);
    g.connect(2, 22);
    g.connect(2, 3);
    g.connect(2, 1001);
    std::ostream_iterator<int> out_it (std::cout, ", ");
    g.neighbors(2, out_it);
    std::cout << g.print() << std::endl;
    ALLPASSED(argv[0]);
    return 0;
}

