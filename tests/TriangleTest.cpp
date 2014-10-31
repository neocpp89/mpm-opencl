#define __CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#undef CL_VERSION_1_2
#include <CL/cl.hpp>
#include <utility>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <vector>
#include <memory>

#include <cmath>

#include "clerror.hpp"
#include "TestUtils.hpp"

#define DEVICE_TYPE_TO_USE  CL_DEVICE_TYPE_ALL

int main(int argc, char ** argv)
{
    size_t platform = 0;
    size_t device = 0;
    const bool verbose = false;

    if (argc > 1) {
        platform = std::stoul(std::string(argv[1]));
    }
    if (argc > 2) {
        device = std::stoul(std::string(argv[2]));
    }

    try {

        cl_int err;
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        // Search for all OpenCL platforms, and use the one selected.
        std::cout << "Number of OpenCL platforms: " << platforms.size() << std::endl;
        for (size_t i = 0; i < platforms.size(); i++) {
            std::cout << "[" << i << "] Platform Name: " <<
                platforms[i].getInfo<CL_PLATFORM_NAME>().c_str()<< std::endl;
        }

        std::cout << "Using platform " << platform << ".\n";
        if (platform >= platforms.size()) {
            std::cerr << "Fatal -- Invalid platform selected." << std::endl;
            exit(EXIT_FAILURE);
        }
        
        cl_context_properties cps[3] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(platforms[platform])(),
            0
        };

        // Create context for all devices in the platform.
        cl::Context context(DEVICE_TYPE_TO_USE, cps);

        // Select the device requested if available.
        std::cout << "Number of devices in platform: " << platforms.size() << std::endl;
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        for (size_t i = 0; i < devices.size(); i++) {
            std::cout << "[" << i << "] Device Name: " <<
                devices[i].getInfo<CL_DEVICE_NAME>().c_str()<< std::endl;
        }

        std::cout << "Using device " << platform << ".\n";
        if (platform >= platforms.size()) {
            std::cerr << "Fatal -- Invalid device selected." << std::endl;
            exit(EXIT_FAILURE);
        }

        const unsigned short p = 2;
        const size_t onedim = 1 << p;
        const size_t numTris = onedim*onedim;

        // Create triange vertices. 
        std::unique_ptr<cl_double2 []> A(new cl_double2[numTris]);
        std::unique_ptr<cl_double2 []> B(new cl_double2[numTris]);
        std::unique_ptr<cl_double2 []> C(new cl_double2[numTris]);
        for (size_t i = 0; i < onedim; i++) {
            for (size_t j = 0; j < onedim; j++) {
                A[(i*onedim + j)].s[0] = i;
                A[(i*onedim + j)].s[1] = j;
                B[(i*onedim + j)].s[0] = i+1;
                B[(i*onedim + j)].s[1] = j;
                C[(i*onedim + j)].s[0] = i+1;
                C[(i*onedim + j)].s[1] = j+1;
            }
        }

        // Create host buffer for results.
        std::unique_ptr<cl_double4 []> sf(new cl_double4[numTris]);
        if (sf == NULL) {
            std::cerr << "FATAL -- can't alloc output buffer." << std::endl;
            exit(1);
        }

        // Create a command queue on the selected device.
        cl::CommandQueue queue = cl::CommandQueue(context, devices[device], 0, &err);

        // copy host data to device
        cl::Buffer d_A = cl::Buffer(context, CL_MEM_READ_ONLY, numTris*sizeof(cl_double2));
        cl::Buffer d_B = cl::Buffer(context, CL_MEM_READ_ONLY, numTris*sizeof(cl_double2));
        cl::Buffer d_C = cl::Buffer(context, CL_MEM_READ_ONLY, numTris*sizeof(cl_double2));

        // Create buffer to hold results
        cl::Buffer d_sf = cl::Buffer(context, CL_MEM_READ_WRITE, numTris*sizeof(cl_double4));
        
        if (verbose) {
            std::cout << "Allocated triangle buffers on device... " << numTris*sizeof(cl_double2) << " bytes. (" << numTris*sizeof(cl_double2)/1048576 << " MB each)" << std::endl;
            if (numTris < 64) {
                std::cout << "A:\n";
                for (size_t i = 0; i < numTris; i++) {
                    std::cout << "{" << A[i].s[0] << ", " << A[i].s[1] << "}\n";
                }
                std::cout << "B:\n";
                for (size_t i = 0; i < numTris; i++) {
                    std::cout << "{" << B[i].s[0] << ", " << B[i].s[1] << "}\n";
                } 
                std::cout << "C:\n";
                for (size_t i = 0; i < numTris; i++) {
                    std::cout << "{" << C[i].s[0] << ", " << C[i].s[1] << "}\n";
                }
            }
        }

        // Copy host data to device buffers.
        queue.enqueueWriteBuffer(d_A, CL_TRUE, 0, numTris*sizeof(cl_double2), A.get());
        queue.enqueueWriteBuffer(d_B, CL_TRUE, 0, numTris*sizeof(cl_double2), B.get());
        queue.enqueueWriteBuffer(d_C, CL_TRUE, 0, numTris*sizeof(cl_double2), C.get());

        std::ifstream sourceFileName;
        sourceFileName.open("MPMKernels.cl");
        if (sourceFileName.fail()) {
            std::cerr << "FATAL -- Couldn't open cl program file." << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string sourceFile(
                        std::istreambuf_iterator<char>( sourceFileName),
                         (std::istreambuf_iterator<char>())
                        );

        cl::Program::Sources mpmkernels_source(1,
                std::make_pair(sourceFile.c_str(),
                                sourceFile.length() +1
                                )
                            );

        cl::Program rotn_program(context, mpmkernels_source);
        err = rotn_program.build(devices);
        std::string buildlog = rotn_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], &err);

        if (verbose) {
            std::cout << "Read Kernel:\n" << sourceFile << std::endl;
            std::cout << "Build log:\n" << buildlog << std::endl;
        }

        cl::Kernel tri2d_sf_kernel(rotn_program, "tri2d_local_coordinates", &err);

        std::vector<std::pair<cl::Buffer, cl_double4>> pointSets;
        pointSets.push_back(std::make_pair(d_A, (cl_double4){1, 0, 0, 0}));
        pointSets.push_back(std::make_pair(d_B, (cl_double4){0, 1, 0, 0}));
        pointSets.push_back(std::make_pair(d_C, (cl_double4){0, 0, 1, 0}));

        tri2d_sf_kernel.setArg(0, (cl_ulong)numTris);
        // tri2d_sf_kernel.setArg(1, d_A); // we will set this later.
        tri2d_sf_kernel.setArg(2, d_A);
        tri2d_sf_kernel.setArg(3, d_B);
        tri2d_sf_kernel.setArg(4, d_C);
        tri2d_sf_kernel.setArg(5, d_sf);

        for (auto const &b : pointSets) {
            tri2d_sf_kernel.setArg(1, b.first);
            queue.enqueueNDRangeKernel(tri2d_sf_kernel, cl::NullRange, cl::NDRange(numTris), cl::NullRange);
            queue.enqueueReadBuffer(d_sf, CL_TRUE, 0, numTris*sizeof(cl_double4), sf.get());
            bool all_points_passed = true;
            const double SF_TOL = 1e-12;
            for (size_t i = 0; i < numTris; i++) {
                for (size_t j = 0; j < 4; j++) {
                    if (std::fabs(static_cast<double>(sf[i].s[j] - b.second.s[j])) > SF_TOL) {
                        all_points_passed = false;
                    }
                }
            }
            TEST_MSG("Expected {" + std::to_string(b.second.s[0]) + "," +
                std::to_string(b.second.s[1]) + "," +
                std::to_string(b.second.s[2]) + "," +
                std::to_string(b.second.s[3]) + "}"
                , all_points_passed);
        }

        if (verbose) {
            if (numTris < 64) {
                std::cout << "SF:\n";
                for (size_t i = 0; i < numTris; i++) {
                    std::cout << "{" << sf[i].s[0] << ", " << sf[i].s[1] << ", " << sf[i].s[2] << ", " << sf[i].s[3] << "}\n";
                }
            }
        }

    } catch(cl::Error err) {
        std::cerr << "OpenCL Error: " << err.what() << " (" << CLErrorMap.at(err.err()) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

