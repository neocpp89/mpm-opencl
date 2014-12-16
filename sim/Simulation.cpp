#define __CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#undef CL_VERSION_1_2
#include <CL/cl.hpp>

#include <vector>
#include <string>
#include <exception>
#include <ostream>
#include <fstream>
#include <memory>
#include <map>
#include <string>

#include "Configuration.hpp"
#include "Simulation.hpp"
#include "Writer.hpp"
#include "gzstream.hpp"
#include "Tokenizer.hpp"

namespace mpm
{

void Simulation::initializeOpenCL(size_t platform, size_t device)
{
    cl_int err;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Search for all OpenCL platforms, and use the one selected.
    /*
    std::cout << "Number of OpenCL platforms: " << platforms.size() << std::endl;
    for (size_t i = 0; i < platforms.size(); i++) {
        std::cout << "[" << i << "] Platform Name: " <<
         platforms[i].getInfo<CL_PLATFORM_NAME>().c_str()<< std::endl;
    }
    */

    // std::cout << "Using platform " << platform << ".\n";
    if (platform >= platforms.size()) {
        // std::cerr << "Fatal -- Invalid platform selected." << std::endl;
        throw std::runtime_error("Invalid OpenCL platform.");
    }

    cl_context_properties cps[3] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)(platforms[platform])(),
        0
    };

    // Create context for all devices in the platform.
    clContext = std::unique_ptr<cl::Context>
        (new cl::Context(CL_DEVICE_TYPE_ALL, cps));
    std::vector<cl::Device> devices = clContext->getInfo<CL_CONTEXT_DEVICES>();

    if (device >= devices.size()) {
        throw std::runtime_error("Invalid OpenCL device.");
    }

    // Create command queue for the device.
    clQueue = std::unique_ptr<cl::CommandQueue>
        (new cl::CommandQueue(*clContext, devices[device], 0));

    return;
}

void Simulation::copyToOpenCLDevice()
{
    return;
}

void Simulation::copyFromOpenCLDevice()
{
    return;
}

void Simulation::preprocess()
{
    return;
}

void Simulation::readMesh(const std::string &filename)
{
    return;
}

void Simulation::readParticles(std::istream &is)
{
    if (is.fail()) {
        throw std::runtime_error("Invalid input stream.");
    }

    auto header = Tokenizer::splitNextLine(is, ',');

    while (true) {
        auto row = Tokenizer::splitNextLine(is, ',');
        if (is.fail()) {
            break;
        }
        if (row.size() != header.size()) {
            throw std::runtime_error("Error reading particle file; field("
                + std::to_string(header.size()) + ") and value("
                + std::to_string(row.size()) + ") sizes differ.");
        }

        std::map<const std::string, std::string> m;
        for (size_t i = 0; i < header.size(); i++) {
            m[header[i]] = row[i];
        }

        // the default values from the map will be used if
        // the keys aren't defined.
        size_t id = std::stoul(m["id"]);

        MaterialPoints.push_back(MaterialPoint3<double>(id));
        auto &p = MaterialPoints.back();

        p.Position.x = std::stod(m["x"]);
        p.Position.y = std::stod(m["y"]);
        p.Position.z = std::stod(m["z"]);

        p.Velocity.x = std::stod(m["x_t"]);
        p.Velocity.y = std::stod(m["y_t"]);
        p.Velocity.z = std::stod(m["z_t"]);

        p.CauchyStress(0,0) = std::stod(m["Txx"]);
        p.CauchyStress(0,1) = std::stod(m["Txy"]);
        p.CauchyStress(0,2) = std::stod(m["Txz"]);
        p.CauchyStress(1,1) = std::stod(m["Tyy"]);
        p.CauchyStress(1,2) = std::stod(m["Tyz"]);
        p.CauchyStress(2,2) = std::stod(m["Tzz"]);
        p.CauchyStress(1,0) = p.CauchyStress(0,1);
        p.CauchyStress(2,0) = p.CauchyStress(0,2);
        p.CauchyStress(2,1) = p.CauchyStress(1,2);

        p.TimeAveragedCauchyStress(0,0) = std::stod(m["Txx_bar"]);
        p.TimeAveragedCauchyStress(0,1) = std::stod(m["Txy_bar"]);
        p.TimeAveragedCauchyStress(0,2) = std::stod(m["Txz_bar"]);
        p.TimeAveragedCauchyStress(1,1) = std::stod(m["Tyy_bar"]);
        p.TimeAveragedCauchyStress(1,2) = std::stod(m["Tyz_bar"]);
        p.TimeAveragedCauchyStress(2,2) = std::stod(m["Tzz_bar"]);
        p.TimeAveragedCauchyStress(1,0) = p.TimeAveragedCauchyStress(0,1);
        p.TimeAveragedCauchyStress(2,0) = p.TimeAveragedCauchyStress(0,2);
        p.TimeAveragedCauchyStress(2,1) = p.TimeAveragedCauchyStress(1,2);

        // delete the known keys (above), add everything else in the map
        // as a state variable.
        for (auto const &known : {
                "id",
                "x", "y", "z",
                "x_t", "y_t", "z_t",
                "Txx", "Txy", "Txz", "Tyy", "Tyz", "Tzz",
                "Txx_bar", "Txy_bar", "Txz_bar", "Tyy_bar", "Tyz_bar", "Tzz_bar"
            }) {
            m.erase(known);
        }
        for (auto const &kv : m) {
            p.StateVariables[kv.first] = std::stod(kv.second);
        }
    }

    return;
}

void Simulation::readParticles(const std::string &filename, bool compressed)
{
    if (compressed) {
        igzstream is(filename.c_str());
        readParticles(is);
    } else {
        std::ifstream is(filename);
        readParticles(is);
    }

    return;
}

void Simulation::readConfigFile(const std::string &filename)
{
    StaticConfiguration sc;

    // What key-value pairs we expect, and their defaults
    sc.addStringOption("MeshFile", "mesh.msh");
    sc.addStringOption("ParticleFile", "particles.csv");
    sc.addStringOption("OutputDirectory", ".");

    sc.readConfigFile(filename);

    readParticles(sc.getStringOption("ParticleFile"));
    readMesh(sc.getStringOption("MeshFile"));

    return;
}

void Simulation::writeParticles(bool hexformat) const
{
    std::string particleFile(OutputDirectory
        + "/fp_" + std::to_string(currentFrame) + ".csv.gz");
    auto begin = MaterialPoints.begin();
    auto end = MaterialPoints.end();

    ogzstream os(particleFile.c_str());
    os << (*begin).getCSVHeaderString() << '\n';

//    if (hexformat) {
        while (begin != end) {
            os << (*begin).getCSVHexRowString() << '\n';
            begin++;
        }
/*    } else {
        while (begin != end) {
            os << (*begin).getCSVRowString() << '\n';
            begin++;
        }
    }
*/
    return;
}

void Simulation::writeMesh() const
{
    return;
}

void Simulation::writeFrame(bool outputParticles, bool outputElements) const
{
    if (outputParticles) {
        writeParticles();
    }
    if (outputElements) {
        writeMesh();    
    }

    return;
}

std::ostream &operator<<(std::ostream &os, const Simulation &s)
{
    os << "Output Directory: " << s.OutputDirectory << '\n';
    return os;
}

}
