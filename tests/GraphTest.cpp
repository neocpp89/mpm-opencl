#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

#include "catch.hpp"
#include "Graph.hpp"
#include "Timer.hpp"

TEST_CASE("Graph Initialization", "[graph][short]")
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);
    g.addEdge(2, 10);
    g.addEdge(2, 22);
    g.addEdge(2, 3);
    g.addEdge(2, 1001);

    REQUIRE(g.degree(0) == 2);
    REQUIRE(g.degree(1) == 2);
    REQUIRE(g.degree(2) == 5);
    REQUIRE(g.degree(3) == 1);
    REQUIRE(g.degree(10) == 2);
    REQUIRE(g.degree(22) == 1);
    REQUIRE(g.degree(1001) == 1);

    std::vector<int> neighbors2;
    g.neighbors(2, std::back_inserter(neighbors2));
    std::sort(neighbors2.begin(), neighbors2.end());
    REQUIRE(neighbors2 == std::vector<int>({1,3,10,22,1001}));
}

TEST_CASE("Graph Copy", "[graph][short]")
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);

    std::vector<int> neighbors2;
    g.neighbors(2, std::back_inserter(neighbors2));
    std::sort(neighbors2.begin(), neighbors2.end());
    REQUIRE(neighbors2 == std::vector<int>({1}));

    // create a copy of the existing graph
    auto h = g;
    
    std::vector<int> neighbors2_h;
    h.neighbors(2, std::back_inserter(neighbors2_h));
    REQUIRE(neighbors2 == neighbors2_h);
}

TEST_CASE("Graph Edge Deletion", "[graph][short]")
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);
    g.addEdge(2, 10);
    g.addEdge(2, 22);
    g.addEdge(2, 3);
    g.addEdge(2, 1001);

    // delete edge on original graph
    g.disconnect(2, 1001);
    std::vector<int> deln2;
    g.neighbors(2, std::back_inserter(deln2));
    std::sort(deln2.begin(), deln2.end());
    REQUIRE(deln2 == std::vector<int>({1,3,10,22}));
}

TEST_CASE("Graph Vertex Deletion", "[graph][short]")
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);
    g.addEdge(2, 10);
    g.addEdge(2, 22);
    g.addEdge(2, 3);
    g.addEdge(2, 1001);

    auto h = g;

    // delete vertex entirely
    g.deleteVertex(2);
    REQUIRE(g.degree(2) == 0);
}

TEST_CASE("Graph Coloring (Greedy)", "[graph][short")
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);
    g.addEdge(2, 10);
    g.addEdge(2, 22);
    g.addEdge(2, 3);
    g.addEdge(2, 1001);

    size_t colors;
    auto m = g.greedyColoring(colors);

    // make sure no vertex has the same color as any of its neighbors.
    for (auto const &k : m) {
        std::vector<int> vn;
        g.neighbors(k.first, std::back_inserter(vn));
        for (auto const &v : vn) {
            REQUIRE(m[k.first] != m[v]);
        }
    }
}

TEST_CASE("Graph Coloring (Smallest Degree Last)", "[graph][short")
{
    UndirectedGraph<int> g;
    g.addEdge(0, 10);
    g.addEdge(0, 1);
    g.addEdge(2, 1);
    g.addEdge(2, 10);
    g.addEdge(2, 22);
    g.addEdge(2, 3);
    g.addEdge(2, 1001);

    size_t colors;
    auto m = g.smallestDegreeLastColoring(colors);

    // make sure no vertex has the same color as any of its neighbors.
    for (auto const &k : m) {
        std::vector<int> vn;
        g.neighbors(k.first, std::back_inserter(vn));
        for (auto const &v : vn) {
            REQUIRE(m[k.first] != m[v]);
        }
    }
}

