#ifndef MSH_HPP
#define MSH_HPP
#include <istream>
#include <sstream>
#include <vector>
#include <map>

namespace msh {
    struct Node {
        std::vector<size_t> physicalIds;
        double x, y, z;
        size_t id;
    };

    struct Element {
        std::vector<size_t> nodeIds;
        std::vector<size_t> physicalIds;
        std::vector<size_t> tags;
        size_t type;
        size_t id;
    };

    class Mesh {
        private:
            size_t numNodes;
            std::map<size_t, struct Node> Nodes;

            size_t numElements;
            std::map<size_t, struct Element> Elements;

            double version;
            size_t filetype;
            size_t datatype;

            void parseMeshFormatSection(std::stringstream &section);
            void parseNodesSection(std::stringstream &section);
            void parseElementsSection(std::stringstream &section);

        public:
            Mesh() { return; } 
            Mesh(std::istream &input) { readMshFile(input); return; }

            void readMshFile(std::istream &input);
    };
};


#endif // MSH_HPP
