#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <string>
#include <vector>
#include <ostream>

#include "MaterialPoint.hpp"
#include "Configuration.hpp"
#include "Msh.hpp"

namespace mpm {

class Simulation
{
    private:
        std::string OutputDirectory;
        msh::Mesh Mesh;
        std::vector<MaterialPoint3<double>> MaterialPoints;
 
    public:
        Simulation() : OutputDirectory(".") { return; } 
        void readConfigFile(const std::string &filename);
        void copyToOpenCLDevice() const;
        void copyFromOpenCLDevice();

        friend std::ostream& operator<<(std::ostream &os, const Simulation &s);
};

}

#endif // SIMULATION_HPP
