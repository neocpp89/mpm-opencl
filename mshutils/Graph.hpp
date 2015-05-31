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

        // returns a map with the key being the vertex and the value being the color
        // color is from 0 to (num_colors - 1)
        // Balanced coloring tries to split the elements up evenly, so the number
        // of colors may be suboptimal.
        std::unordered_map<Integral, size_t> balancedColoring(size_t &num_colors,
            std::vector<Integral> &orderedVertices) const
        {
            std::unordered_map<Integral, size_t> m;
            std::vector<size_t> number_of_vertices_per_color;
            std::vector<bool> sieve;

            num_colors = 0;
            for (auto const &vertex : orderedVertices) {
                // tests which colors are occupied (true if occupied)
                size_t v_degree = degree(vertex);
                while (v_degree >= sieve.size()) {
                    sieve.push_back(false);
                }
                while (v_degree >= number_of_vertices_per_color.size()) {
                    number_of_vertices_per_color.push_back(0);
                }
                std::fill(sieve.begin(), sieve.end(), false);

                std::vector<Integral> vneighbors(v_degree);
                neighbors(vertex, vneighbors.begin());

                for (auto const n : vneighbors) {
                    try {
                        size_t n_color = m.at(n);
                        sieve[n_color] = true;
                    } catch (std::out_of_range &e) {
                        // not in the map yet; ignore it.
                    }
                }

                size_t min_color_idx = 0;
                size_t least_occupied_color = 0;
                size_t new_color_idx = number_of_vertices_per_color.size();
                for (size_t i = 0; i < sieve.size(); i++) {
                    if (sieve[i] == false) {
                        if (number_of_vertices_per_color[i] == 0 && i <= new_color_idx) {
                            new_color_idx = i;
                        } else {
                            if (least_occupied_color == 0 || number_of_vertices_per_color[i] <
                                least_occupied_color) {
                                min_color_idx = i;
                                least_occupied_color = number_of_vertices_per_color[i];
                            }
                        }
                    }
                }

                size_t color = 0;
                if (least_occupied_color == 0) {
                    color = new_color_idx;
                } else {
                    color = min_color_idx;
                }

                if (color > num_colors) {
                    num_colors = color;
                }

                m.insert(std::make_pair(vertex, color));
                number_of_vertices_per_color[color]++;
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

        std::unordered_map<Integral, size_t> smallestDegreeLastColoring(size_t &num_colors, bool balanced = false) const
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
                    deglists.push_back(std::unordered_set<Integral>());
                }
                deglists[deg].insert(vertex);
            }

            size_t num_left = v.size();
            const size_t deg = deglists.size()-1;
            std::vector<Integral> sdl_order;
            while (num_left > 0) {
                auto p = deglists.begin();
                while ((*p).size() == 0) {
                    p++;
                    if (p == deglists.end()) {
                        throw std::runtime_error("No elements left to color.");
                    }
                }
                auto &s = *p;
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

            std::reverse(sdl_order.begin(), sdl_order.end());

            if (sdl_order.size() != v.size()) {
                throw std::runtime_error("wrong size!");
            }

            // use the SDL ordering with balanced or greedy coloring.
            if (balanced) {
                return balancedColoring(num_colors, sdl_order);
            } else {
                return greedyColoring(num_colors, sdl_order);
            }
        }
};

#endif //GRAPH_HPP
