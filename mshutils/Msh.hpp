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
    std::vector<size_t> ElementIds; // which elements this node is a part of
    double x, y, z;
    size_t id;
};
std::ostream & operator<<(std::ostream &os, const Node &elem);

struct Element {
    std::vector<size_t> NodeIds; // which nodes make up this element
    std::vector<size_t> PhysicalIds;
    std::vector<size_t> Tags;
    size_t type;
    size_t id;
    size_t color;
};
std::ostream & operator<<(std::ostream &os, const Element &elem);

struct Triangle2D : public Element {
    double detJ;
    double J[3][2]; //3 shapefunctions with 2 components each
};

class Mesh {
    private:
        std::map<size_t, struct Node> Nodes;
        std::map<size_t, struct Element> Elements;
        UndirectedGraph<size_t> ElementGraph;

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
        void writeMT2(std::ostream &output);
};

}


#endif // MSH_HPP
