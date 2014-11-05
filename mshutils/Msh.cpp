#include <map>
#include <algorithm>
#include <istream>
#include <iostream>
#include <string>
#include <exception>

#include "Msh.hpp"

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

void msh::Mesh::parseMeshFormatSection(std::stringstream &section)
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

void msh::Mesh::parseNodesSection(std::stringstream &section)
{
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

void msh::Mesh::parseElementsSection(std::stringstream &section)
{
    section >> numElements;
    for (size_t i = 0; i < numNodes; i++) {
        if (section.fail()) {
            throw std::runtime_error("Wrong number of elements in msh file.");
        }
        size_t element_id;
        section >> element_id;
        Elements[element_id].id = element_id;
    }
    return;
}

void msh::Mesh::readMshFile(std::istream &input)
{
    bool inSection = false;
    MshSections currentSection;
    std::stringstream ss;
    while (!input.fail()) {
        std::string s;
        std::getline(input, s);
        if (inSection) {
            // Check if the section ended and send to a specialized parser.
            if (s.substr(0, 4) == "$End") {
                switch (currentSection) {
                    case MshSections::MeshFormat:
                        parseMeshFormatSection(ss);
                        break;
                    case MshSections::Nodes:
                        parseNodesSection(ss);
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

