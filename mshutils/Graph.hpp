#ifndef GRAPH_HPP
#define GRAPH_HPP
#include <map>
#include <utility>
#include <string>

template <typename Integral = size_t>
class UndirectedGraph
{
    private:
        std::multimap<Integral, Integral> AdjacencyList;

    public:
        // Connects nodes a and b.
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
                }
                begin++;
            }

            // it doesn't exist, so add it (symmetrically).
            if (!connectionExists) {
                AdjacencyList.insert(std::make_pair(a,b));
                AdjacencyList.insert(std::make_pair(b,a));
            }
        }

        template <typename Iterator>
        void neighbors(Integral a, Iterator output)
        {
            auto k = AdjacencyList.equal_range(a);
            auto begin = k.first;
            auto end = k.second;
            while (begin != end) {
                *output = begin->second;
                begin++;
                output++;
            }
        }

        std::string print()
        {
            std::string s = "[";
            for (auto const &k : AdjacencyList) {
                s = s + "\n\t" + std::to_string(k.first) + ", " + std::to_string(k.second);
            }
            s = s + "\n]";
            return s;
        }
};

#endif //GRAPH_HPP
