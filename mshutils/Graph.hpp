#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <utility>
#include <string>
#include <algorithm>

template <typename Integral = size_t>
class UndirectedGraph
{
    private:
        std::unordered_multimap<Integral, Integral> AdjacencyList;

        void unidirectionalDisconnect(Integral from, Integral which)
        {
            // delete 'which' from 'from's connection list.
            auto k = AdjacencyList.equal_range(from);
            auto begin = k.first;
            auto end = k.second;
            while (begin != end) {
                if ((*begin).second == which) {
                    AdjacencyList.erase(begin);
                    break;
                }
                begin++;
            }

            return;
        }

    public:
        void addEdge(Integral a, Integral b)
        {
            connect(a, b);
            return;
        }

        // Connects nodes a and b (and creates them if they don't exist).
        void connect(Integral a, Integral b)
        {
            if (a == b) {
                // loops are not supported right now.
                return;
            }

            // check if connection already exists.
            auto k = AdjacencyList.equal_range(a);
            auto begin = k.first;
            auto end = k.second;
            bool connectionExists = false;
            while (begin != end) {
                if ((*begin).second == b) {
                    connectionExists = true;
                    break;
                }
                begin++;
            }

            // it doesn't exist, so add it (symmetrically).
            if (!connectionExists) {
                AdjacencyList.insert(std::make_pair(a,b));
                AdjacencyList.insert(std::make_pair(b,a));
            }

            return;
        }

        void deleteEdge(Integral a, Integral b)
        {
            disconnect(a, b);
            return;
        }

        void disconnect(Integral a, Integral b)
        {
            unidirectionalDisconnect(a, b);
            unidirectionalDisconnect(b, a);
            return;
        }

        template <typename Iterator>
        void neighbors(Integral a, Iterator output) const
        {
            auto k = AdjacencyList.equal_range(a);
            auto begin = k.first;
            auto end = k.second;
            while (begin != end) {
                *output = begin->second;
                begin++;
                output++;
            }

            return;
        }

        template <typename Iterator>
        void vertices(Iterator output) const
        {
            std::unordered_set<Integral> s;
            for (auto const &k : AdjacencyList) {
                s.insert(k.first);
            }

            for (auto const &k : s) {
                *output = k;
                output++;
            }

            return;
        }

        size_t degree(Integral a) const
        {
            auto k = AdjacencyList.equal_range(a);
            auto begin = k.first;
            auto end = k.second;
            return std::distance(begin, end);
        }

        void deleteVertex(Integral a)
        {
            std::vector<Integral> v;
            std::back_insert_iterator<std::vector<Integral>> vback(v);
            neighbors(a, vback);
            for (auto const &k : v) {
                disconnect(k, a);
            }
            return;
        }

        std::string print() const
        {
            std::string s = "[";
            for (auto const &k : AdjacencyList) {
                s = s + "\n\t" + std::to_string(k.first) + ", " + std::to_string(k.second);
            }
            s = s + "\n]";
            return s;
        }

        // returns a map with the key being the vertex and the value being the color
        // color is from 0 to (num_colors - 1)
        std::unordered_map<Integral, size_t> greedyColoring(size_t &num_colors) const
        {
            std::vector<Integral> v;
            vertices(std::back_inserter(v));

            // just use the order we were given
            return greedyColoring(num_colors, v);
        }

        // returns a map with the key being the vertex and the value being the color
        // color is from 0 to (num_colors - 1)
        std::unordered_map<Integral, size_t> greedyColoring(size_t &num_colors,
            std::vector<Integral> &orderedVertices) const
        {
            std::unordered_map<Integral, size_t> m;
            num_colors = 0;

            for (auto const &vertex : orderedVertices) {
                // tests which colors are occupied (true if occupied)
                size_t v_degree = degree(vertex);
                std::vector<bool> sieve(v_degree + 1);
                std::fill(sieve.begin(), sieve.end(), false);

                std::vector<Integral> vneighbors(v_degree);
                neighbors(vertex, vneighbors.begin());

                size_t min_color = 0;
                for (auto const n : vneighbors) {
                    try {
                        size_t n_color = m.at(n);
                        sieve[n_color] = true;
                    } catch (std::out_of_range &e) {
                        // not in the map yet; ignore it.
                    }
                }


                for (size_t i = 0; i < sieve.size(); i++) {
                    if (sieve[i] == false) {
                        min_color = i;
                        break;
                    }
                }

                if (min_color > num_colors) {
                    num_colors = min_color;
                }

                m.insert(std::make_pair(vertex, min_color));
            }
            
            num_colors++;

            return m;
        }

        std::unordered_map<Integral, size_t> largestDegreeFirstColoring(size_t &num_colors) const
        {
            std::vector<Integral> v;
            vertices(std::back_inserter(v));

            // implement the algorithm from matula and beck 1983
            std::vector<std::unordered_set<Integral>> deglists;
            std::unordered_map<Integral, bool> eliminated;
            // std::fill(eliminated.begin(), eliminated.end(), false);
            for (auto const &vertex : v) {
                eliminated[vertex] = false;
                size_t deg = degree(vertex);
                while (deg >= deglists.size()) {
                    deglists.push_back({});
                }
                deglists[deg].insert(vertex);
            }

            size_t num_left = v.size();
            std::vector<Integral> sdl_order;
            while (num_left > 0) {
                auto &s = deglists.back();
                size_t deg = deglists.size()-1;
                if (s.size() == 0) {
                    deglists.pop_back();
                } else {
                    auto const vertex = *(s.begin());
                    sdl_order.push_back(vertex);
                    s.erase(vertex);
                    eliminated[vertex] = true;
                    std::vector<Integral> vneighbors(degree(vertex));
                    neighbors(vertex, vneighbors.begin());
                    for (auto const &n: vneighbors) {
                        if (eliminated[n]) {
                            continue;
                        }

                        for (size_t trial_deg = deg; trial_deg > 0; trial_deg--) {
                            if (deglists[trial_deg].erase(n) == 1) {
                                deglists[trial_deg-1].insert(n);
                                break;
                            }
                        }
                    }
                    num_left--;
                }
            }

            if (sdl_order.size() != v.size()) {
                throw std::runtime_error("wrong size!");
            }

            // use the SDL ordering with greedy coloring.
            return greedyColoring(num_colors, sdl_order);
        }

        std::unordered_map<Integral, size_t> smallestDegreeLastColoring(size_t &num_colors) const
        {
            std::vector<Integral> v;
            vertices(std::back_inserter(v));

            // implement the algorithm from matula and beck 1983
            std::vector<std::unordered_set<Integral>> deglists;
            std::unordered_map<Integral, bool> eliminated;
            // std::fill(eliminated.begin(), eliminated.end(), false);
            for (auto const &vertex : v) {
                eliminated[vertex] = false;
                size_t deg = degree(vertex);
                while (deg >= deglists.size()) {
                    deglists.push_back({});
                }
                deglists[deg].insert(vertex);
            }

            size_t num_left = v.size();
            std::vector<Integral> sdl_order;
            while (num_left > 0) {
                auto &s = deglists[0];
                size_t deg = deglists.size()-1;
                if (s.size() == 0) {
                    deglists.erase(deglists.begin());
                } else {
                    auto const vertex = *(s.begin());
                    sdl_order.push_back(vertex);
                    s.erase(vertex);
                    eliminated[vertex] = true;
                    std::vector<Integral> vneighbors(degree(vertex));
                    neighbors(vertex, vneighbors.begin());
                    for (auto const &n: vneighbors) {
                        if (eliminated[n]) {
                            continue;
                        }

                        for (size_t trial_deg = deg; trial_deg > 0; trial_deg--) {
                            if (deglists[trial_deg].erase(n) == 1) {
                                deglists[trial_deg-1].insert(n);
                                break;
                            }
                        }
                    }
                    num_left--;
                }
            }

            std::reverse(sdl_order.begin(), sdl_order.end());

            if (sdl_order.size() != v.size()) {
                throw std::runtime_error("wrong size!");
            }

            // use the SDL ordering with greedy coloring.
            return greedyColoring(num_colors, sdl_order);
        }
};

#endif //GRAPH_HPP
