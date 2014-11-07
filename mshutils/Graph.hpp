#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <vector>
#include <unordered_map>
#include <set>
#include <utility>
#include <string>

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
            std::set<Integral> s;
            for (auto const &k : AdjacencyList) {
                s.insert(k.first);
            }

            for (auto const &k : s) {
                *output = k;
                output++;
            }

            return;
        }

        size_t degree(Integral a)
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

        // returns a map with the key being the node and the value being the color
        // color is from 0 to (num_colors - 1)
        std::unordered_map<Integral, size_t> greedyColoring(size_t &num_colors)
        {

            std::unordered_map<Integral, size_t> m;
            num_colors = 0;

            std::vector<Integral> v;
            std::back_insert_iterator<std::vector<Integral>> vback(v);
            vertices(vback);

            for (auto const &vertex : v) {
                // tests which colors are occupied (true if occupied)
                std::vector<bool> sieve;
                for (size_t i = 0; i <= degree(vertex); i++) {
                    sieve.push_back(false);
                }

                std::vector<Integral> vneighbors;
                std::back_insert_iterator<std::vector<Integral>> vneighborsback(vneighbors);
                neighbors(vertex, vneighborsback);

                size_t min_color = 0;
                for (auto const &n : vneighbors) {
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
};

#endif //GRAPH_HPP
