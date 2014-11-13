#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#define __CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#undef CL_VERSION_1_2
#include <CL/cl.hpp>

#include <string>
#include <vector>
#include <ostream>
#include <memory>

#include "MaterialPoint.hpp"
#include "Configuration.hpp"
#include "Msh.hpp"

namespace mpm {

class Simulation
{
    private:
        std::unique_ptr<cl::Context> clContext;
        std::unique_ptr<cl::CommandQueue> clQueue;

        std::string OutputDirectory;
        msh::Mesh Mesh;
        std::vector<MaterialPoint3<double>> MaterialPoints;

        double currentTime;
        size_t currentFrame;

        void readParticles(const std::string &filename, bool compressed = false);
        void readMesh(const std::string &filename);
        void writeParticles(bool hexformat = true) const;
        void writeMesh() const;
 
    public:
        Simulation() :
            OutputDirectory("."),
            currentTime(0),
            currentFrame(0)
        {
            return;
        }
        void readConfigFile(const std::string &filename);

        // select the first device on the first opencl platform by default
        void initializeOpenCL(size_t platform = 0, size_t device = 0);

        void preprocess();

        void copyToOpenCLDevice() const;
        void copyFromOpenCLDevice();

        void writeFrame(bool outputParticles = true, bool outputElements = false) const;

        friend std::ostream& operator<<(std::ostream &os, const Simulation &s);
};

}

#endif // SIMULATION_HPP
