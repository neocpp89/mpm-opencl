#ifndef MSH_HPP
#define MSH_HPP
#include <istream>
#include <ostream>
#include <sstream>
#include <vector>
#include <map>

#include "Graph.hpp"

namespace msh {
    struct Node {
        std::vector<size_t> PhysicalIds;
        double x, y, z;
        size_t id;
    };
    std::ostream & operator<<(std::ostream &os, const Node &elem);

    struct Element {
        std::vector<size_t> NodeIds;
        std::vector<size_t> PhysicalIds;
        std::vector<size_t> Tags;
        size_t type;
        size_t id;
        size_t color;
    };
    std::ostream & operator<<(std::ostream &os, const Element &elem);

    class Mesh {
        private:
            std::map<size_t, struct Node> Nodes;
            std::map<size_t, struct Element> Elements;

            double version;
            size_t filetype;
            size_t datatype;

            void parseMeshFormatSection(std::stringstream &section);
            void parseNodesSection(std::stringstream &section);
            void parseElementsSection(std::stringstream &section);

            void colorElements();

        public:
            Mesh() { return; } 
            Mesh(std::istream &input)
            {
                readMshFile(input);
                colorElements();
                return;
            }

            void readMshFile(std::istream &input);
    };

};


#endif // MSH_HPP
