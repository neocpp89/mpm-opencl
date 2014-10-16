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

//! You will need to tweak these 2 parameters
//! Using 0 will always choose the 1st implementation found
#define PLATFORM_TO_USE 0
#define DEVICE_TYPE_TO_USE  CL_DEVICE_TYPE_CPU

int main(int argc, char ** argv)
{

try {

    cl_int err;
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    std::cout << "Number of platforms:\t" << platforms.size() << std::endl;
    for (auto i = platforms.begin(); i != platforms.end(); ++i) {
        // pick a platform and do something
        std::cout << " Platform Name: " << (*i).getInfo<CL_PLATFORM_NAME>().c_str()<< std::endl;
    }

    const unsigned int W = 8192;
    const unsigned int H = 8192;

    const unsigned short p = 12;
    const size_t onedim = 1 << p;
    const size_t numTris = onedim*onedim;

    cl_double2 *A = new cl_double2[numTris];
    cl_double2 *B = new cl_double2[numTris];
    cl_double2 *C = new cl_double2[numTris];

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
    std::cout << "Done initializing array.\n";

    // Homegrown function to read a BMP from file
    float* ip = new float[W*H];
    for (size_t i = 0; i < W; i++) {
        for (size_t j = 0; j < H; j++) {
            ip[i*W + j] = (float)j / H;
        }
    }
    float * op = new float[W*H];
    float theta = 0.125;

    //Lets choose the first platform
    cl_context_properties cps[3] = {


    CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[PLATFORM_TO_USE])(), 0};

    // Select the default platform and create a context
    // using this platform for a GPU type device

    cl::Context context(DEVICE_TYPE_TO_USE, cps);

    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

    //Lets create a command queue on the first device
    cl::CommandQueue queue = cl::CommandQueue(context, devices[0], 0, &err);

    //[H3] Step2 â Declare Buffers and Move Data

    //We assume that the input image is the array âipâ
    //and the angle of rotation is theta
    float cos_theta = cos(theta);
    float sin_theta = sin(theta);

    cl::Buffer d_ip = cl::Buffer(context, CL_MEM_READ_ONLY, W*H* sizeof(float));
    cl::Buffer d_op = cl::Buffer(context, CL_MEM_READ_WRITE, W*H* sizeof(float));
    queue.enqueueWriteBuffer(d_ip, CL_TRUE, 0, W*H* sizeof(float), ip);
    std::cout << "copied original data... " << W*H*sizeof(float) << " bytes." << std::endl;

    // copy host data to gpu
    cl::Buffer d_A = cl::Buffer(context, CL_MEM_READ_ONLY, numTris*sizeof(cl_double2));
    cl::Buffer d_B = cl::Buffer(context, CL_MEM_READ_ONLY, numTris*sizeof(cl_double2));
    cl::Buffer d_C = cl::Buffer(context, CL_MEM_READ_ONLY, numTris*sizeof(cl_double2));
    std::cout << "alloc'd triangle buffers... " << numTris*sizeof(cl_double2) << " bytes." << std::endl;
    queue.enqueueWriteBuffer(d_A, CL_TRUE, 0, numTris*sizeof(cl_double2), A);
    queue.enqueueWriteBuffer(d_B, CL_TRUE, 0, numTris*sizeof(cl_double2), B);
    queue.enqueueWriteBuffer(d_C, CL_TRUE, 0, numTris*sizeof(cl_double2), C);
    std::cout << "copied triangle data... " << numTris*sizeof(cl_double2) << " bytes." << std::endl;

    cl::Buffer d_sf = cl::Buffer(context, CL_MEM_READ_WRITE, numTris*sizeof(cl_double4));
    //[H3]Step3 â Runtime kernel compilation
    std::ifstream sourceFileName;
    sourceFileName.open("rotation.cl");
    if (sourceFileName.fail()) {
        std::cerr << "FATAL -- Couldn't open cl program file." << std::endl;
        exit(1);
    }
    std::string sourceFile(
                    std::istreambuf_iterator<char>( sourceFileName),
                     (std::istreambuf_iterator<char>())
                    );

    //std::cout<<sourceFile;

    cl::Program::Sources rotn_source(1,
            std::make_pair(sourceFile.c_str(),
                            sourceFile.length() +1
                            )
                        );

    cl::Program rotn_program(context, rotn_source);
    err = rotn_program.build(devices);;
    std::string buildlog = rotn_program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], &err);
    std::cout << "Read Kernel:\n" << sourceFile << std::endl;
    std::cout << "Build log:\n" << buildlog << std::endl;

    cl::Kernel rotn_kernel(rotn_program, "img_rotate", &err);
    cl::Kernel tri2d_sf_kernel(rotn_program, "tri2d_local_coordinates", &err);

    //[H3]Step4 â Run the program
    rotn_kernel.setArg(0, d_op);
    rotn_kernel.setArg(1, d_ip);
    rotn_kernel.setArg(2, W);
    rotn_kernel.setArg(3, H);
    rotn_kernel.setArg(4, sin_theta);
    rotn_kernel.setArg(5, cos_theta);


    tri2d_sf_kernel.setArg(0, numTris);
    tri2d_sf_kernel.setArg(1, d_A);
    tri2d_sf_kernel.setArg(2, d_A);
    tri2d_sf_kernel.setArg(3, d_B);
    tri2d_sf_kernel.setArg(4, d_C);
    tri2d_sf_kernel.setArg(5, d_sf);

    // Run the kernel on specific ND range
    cl::NDRange globalws(W,H);
    //In this example the local work group size is not important because
    //there is no communication between local work items

    queue.enqueueNDRangeKernel(rotn_kernel, cl::NullRange, globalws, cl::NullRange);
     //[H3]Step5 â Read result back to host
    // Read buffer d_op into a local op array
    queue.enqueueReadBuffer(d_op, CL_TRUE, 0, W*H*sizeof(float), op);

    std::cout << "Done rotating image.\n";

    cl::NDRange trirange(numTris);
    for (size_t i = 0; i < 100; i++) {
        queue.enqueueNDRangeKernel(tri2d_sf_kernel, cl::NullRange, trirange, cl::NullRange);
    }

    cl_double4 *sf = new cl_double4[numTris];
    if (sf == NULL) {
        std::cerr << "FATAL -- can't alloc output buffer." << std::endl;
        exit(1);
    }
    queue.enqueueReadBuffer(d_sf, CL_TRUE, 0, numTris*sizeof(cl_double4), sf);
    std::cout << "Done calculating local coords.\n";

}
catch(cl::Error err)
{
   std::cout << err.what() << "(" << err.err() << ")" << std::endl;
}

}

