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

    auto h = g;
    g.disconnect(2, 1001);
    std::cout << "deg " << g.degree(2) << '\n';
    std::cout << g.print() << std::endl;
    g.deleteVertex(2);
    std::cout << "deg " << g.degree(2) << '\n';
    std::cout << g.print() << std::endl;

    h.vertices(out_it);

    size_t colors;
    auto m = h.greedyColoring(colors);
    for (auto const &kv : m) {
        std::cout << "\n\t[" << kv.first << ", " << kv.second << "]"; 
    }
    std::cout << '\n';

    std::cout << h.print() << std::endl;
    ALLPASSED(argv[0]);
    return 0;
}

