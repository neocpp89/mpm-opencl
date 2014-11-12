#include <vector>
#include <string>
#include <exception>
#include <ostream>

#include "Configuration.hpp"
#include "Simulation.hpp"

namespace mpm
{
    void Simulation::readConfigFile(const std::string &filename)
    {
        StaticConfiguration sc;
        sc.addStringOption("MeshFile", "mesh.msh");
        sc.addStringOption("ParticleFile", "particles.csv");
        sc.addStringOption("OutputDirectory", ".");
        sc.readConfigFile(filename);
        return;
    }

    std::ostream &operator<<(std::ostream &os, const Simulation &s)
    {
        os << "Output Directory: " << s.OutputDirectory << '\n';
        return os;
    }
}
