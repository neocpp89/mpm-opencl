#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

#include "TestUtils.hpp"
#include "Graph.hpp"
#include "Timer.hpp"

int main(int argc, char **argv)
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);
    g.addEdge(2, 10);
    g.addEdge(2, 22);
    g.addEdge(2, 3);
    g.addEdge(2, 1001);

    TEST(g.degree(0) == 2);
    TEST(g.degree(1) == 2);
    TEST(g.degree(2) == 5);
    TEST(g.degree(3) == 1);
    TEST(g.degree(10) == 2);
    TEST(g.degree(22) == 1);
    TEST(g.degree(1001) == 1);

    std::vector<int> neighbors2;
    g.neighbors(2, std::back_inserter(neighbors2));
    std::sort(neighbors2.begin(), neighbors2.end());
    TEST(neighbors2 == std::vector<int>({1,3,10,22,1001}));

    // create a copy of the existing graph
    auto h = g;

    // delete edge on original graph
    g.disconnect(2, 1001);
    std::vector<int> deln2;
    g.neighbors(2, std::back_inserter(deln2));
    std::sort(deln2.begin(), deln2.end());
    TEST(deln2 == std::vector<int>({1,3,10,22}));

    // delete vertex entirely
    g.deleteVertex(2);
    TEST(g.degree(2) == 0);

    size_t colors;
    {
        Timer s("Graph Coloring");
        auto m = h.greedyColoring(colors);

        // make sure no vertex has the same color as any of its neighbors.
        for (auto const &k : m) {
            std::vector<int> vn;
            h.neighbors(k.first, std::back_inserter(vn));
            for (auto const &v : vn) {
                TEST(m[k.first] != m[v]);
            }
        }
    }

    std::cout << "h = " << h.print() << std::endl;
    ALLPASSED(argv[0]);
    return 0;
}

