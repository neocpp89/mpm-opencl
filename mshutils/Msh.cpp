#include <map>
#include <unordered_map>
#include <algorithm>
#include <istream>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <set>

#include "Graph.hpp"
#include "Msh.hpp"
#include "Timer.hpp"

namespace msh {

enum class MshSections {
    MeshFormat,
    Nodes,
    Elements,
    Periodic,
    PhysicalNames,
    NodeData,
    ElementData,
    ElementNodeData,
    InterpolationScheme,
    Comment // this is actually just a section we don't recognize
};

const std::map<std::string, MshSections> SectionFromStringMap = {
    { "MeshFormat", MshSections::MeshFormat },
    { "Nodes", MshSections::Nodes },
    { "Elements", MshSections::Elements },
    { "Periodic", MshSections::Periodic },
    { "PhysicalNames", MshSections::PhysicalNames },
    { "NodeData", MshSections::NodeData },
    { "ElementData", MshSections::ElementData },
    { "ElementNodeData", MshSections::ElementNodeData },
    { "InterpolationScheme", MshSections::InterpolationScheme },
    { "Comment", MshSections::Comment }
};

std::ostream & operator<<(std::ostream &os, const Node &node)
{
    os << "[\n\tid: " << node.id << '\n';
    os << "\tcoordinates: ( " << node.x << " " << node.y << " " << node.z << " )\n]";
    return os;
}

std::ostream & operator<<(std::ostream &os, const Element &elem)
{
    os << "[\n\tid: " << elem.id << '\n';
    os << "\ttype: " << elem.type << '\n';
    os << "\tcolor: " << elem.color << '\n';
    os << "\tnodes: [ ";
    for (auto const &k : elem.NodeIds) {
        os << k << " ";
    }
    os << "]\n";
    os << "\ttags : [ ";
    for (auto const &k : elem.Tags) {
        os << k << " ";
    }
    os << "]\n";
    os << "]";
    return os;
}

void Mesh::parseMeshFormatSection(std::stringstream &section)
{
    section >> version;
    section >> filetype;
    section >> datatype;

    if (filetype != 0) {
        throw std::runtime_error("Wrong file type (only ASCII supported).");
    }

    if (datatype != sizeof(double)) {
        throw std::runtime_error("Wrong mesh data size.");
    }

    return;
}

void Mesh::parseNodesSection(std::stringstream &section)
{
    size_t numNodes;
    section >> numNodes;
    for (size_t i = 0; i < numNodes; i++) {
        if (section.fail()) {
            throw std::runtime_error("Wrong number of nodes in msh file.");
        }
        size_t node_id;
        section >> node_id;
        
        Nodes[node_id].id = node_id;
        section >> Nodes[node_id].x;
        section >> Nodes[node_id].y;
        section >> Nodes[node_id].z;
    }
    return;
}

void Mesh::parseElementsSection(std::stringstream &section)
{
    size_t numElements;
    section >> numElements;
    for (size_t i = 0; i < numElements; i++) {
        if (section.fail()) {
            throw std::runtime_error("Wrong number of elements in msh file.");
        }
        size_t element_id;
        section >> element_id;

        Elements[element_id].id = element_id;
        section >> Elements[element_id].type;

        size_t num_tags;
        section >> num_tags;
        for (size_t j = 0; j < num_tags; j++) {
            size_t tag;
            section >> tag;
            Elements[element_id].Tags.push_back(tag);
        }

        std::string s;
        std::getline(section, s);
        std::stringstream nodestream;
        nodestream << s;
        while (true) {
            size_t node_id;
            nodestream >> node_id;
            if (nodestream.fail()) {
                break;
            }
            Elements[element_id].NodeIds.push_back(node_id);
        }
        // std::cout << Elements[element_id] << '\n';
    }
    return;
}

void Mesh::readMshFile(std::istream &input)
{
    bool inSection = false;
    MshSections currentSection = MshSections::Comment;
    std::stringstream ss;
    while (!input.fail()) {
        std::string s;
        std::getline(input, s);
        if (inSection) {
            // Check if the section ended and send to a specialized parser.
            if (s.size() >= 5 && s.substr(0, 4) == "$End") {
                switch (currentSection) {
                    case MshSections::MeshFormat:
                        parseMeshFormatSection(ss);
                        break;
                    case MshSections::Nodes:
                        parseNodesSection(ss);
                        break;
                    case MshSections::Elements:
                        parseElementsSection(ss);
                        break;
                    case MshSections::Comment:
                    default:
                        break;
                }
                ss.clear();
                ss.str("");
                inSection = false;
            } else {
                 // otherwise consume stream.
                ss << s << '\n';
            }
        } else {
            // Check if this is the beginning of a section
            if (s[0] == '$') {
                std::string secstr = s.substr(1);
                if (SectionFromStringMap.count(secstr) == 1) {
                    currentSection = SectionFromStringMap.at(secstr);
                    inSection = true;
                }
            }
        }
    }
    return;
}

std::unordered_map<size_t, std::set<size_t>> 
findNeighbors(const std::map<size_t, Element> &elements)
{
    std::unordered_map<size_t, std::vector<size_t>> nodemap;
    for (auto const &el : elements) {
        for (auto const n_id : el.second.NodeIds) {
            nodemap[n_id].push_back(el.second.id);
        }
    }
    // nodemap now contains a map of nodes->elements which include that node

    // For each node, connect every pair of elements that it touches.
    // The set automatically eliminates the possiblilty of duplicates.
    std::unordered_map<size_t, std::set<size_t>> neighbors;
    for (auto const &kv : nodemap) {
        auto v = kv.second;
        for (auto const &a : v) {
            for (auto const &b : v) {
                if (a != b) {
                    neighbors[a].insert(b);
                }
            }
        }
    }

    return neighbors;
}

void Mesh::colorElements()
{
    Timer timeit("Mesh Coloring");
    UndirectedGraph<size_t> g;
    auto neighbors = findNeighbors(Elements);
    for (auto const &kv: neighbors) {
        auto primary = kv.first;
        for (auto const &secondary: kv.second) {
            g.connect(primary, secondary);
        }
    }
    size_t colors;
    {
    Timer x("actual coloring");
    //g.greedyColoring(colors);
    auto m = g.smallestDegreeLastColoring(colors);
    for (auto const &kv : m) {
        Elements[kv.first].color = kv.second;
    }
    }
    std::cout << "Mesh has " << Nodes.size() << " vertices.\n";
    std::cout << "Mesh has " << Elements.size() << " elements.\n";
    std::cout << "Colored mesh with " << colors << " colors.\n";
    // std::cout << g.print();
    return;
}

void Mesh::writeMT2(std::ostream &output)
{
    const size_t buflen = 64;
    char buf[buflen];
    
    for (auto const &kv : Nodes) {
        auto const &node = kv.second;
        output << "N," << node.id << ',';
        std::snprintf(buf, buflen, "%la", node.x);
        output << buf << ',';
        std::snprintf(buf, buflen, "%la", node.y);
        output << buf << ',';
        std::snprintf(buf, buflen, "%la", node.z);
        output << buf << '\n';
    }

    for (auto const &kv : Elements) {
        auto const &element = kv.second;
        if (element.type != 2) {
            continue;
        }
        output << "E," << element.id << ',';
        output << element.color;
        for (auto const &n : element.NodeIds) {
            output << ',' << n;
        }
        output << '\n';
    }
    return;
}

}
