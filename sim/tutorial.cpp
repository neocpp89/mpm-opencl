    #if 0
    #include <iostream>
    #define CL_USE_DEPRECATED_OPENCL_1_1_APIS
    #include <iostream>
    #include <CL/cl.h>
    #undef CL_VERSION_1_2
    #include <CL/cl.hpp>
     
    int main(){
    //get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if(all_platforms.size()==0){
    std::cout<<" No platforms found. Check OpenCL installation!\n";
    exit(1);
    }
    cl::Platform default_platform=all_platforms[0];
    std::cout << "Using platform: "<<default_platform.getInfo<CL_PLATFORM_NAME>()<<"\n";
     
    //get default device of the default platform
    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size()==0){
    std::cout<<" No devices found. Check OpenCL installation!\n";
    exit(1);
    }
    cl::Device default_device=all_devices[0];
    std::cout<< "Using device: "<<default_device.getInfo<CL_DEVICE_NAME>()<<"\n";
     
     
    //cl::Context context({default_device});
     cl::Context context(default_device);
    cl::Program::Sources sources;
     
    // kernel calculates for each element C=A+B
    std::string kernel_code=
    " void kernel simple_add(global const int* A, global const int* B, global int* C){ "
    " C[get_global_id(0)]=A[get_global_id(0)]+B[get_global_id(0)]; "
    " } ";
    sources.push_back({kernel_code.c_str(),kernel_code.length()});
     
    cl::Program program(context,sources);
    if(program.build({default_device})!=CL_SUCCESS){
    std::cout<<" Error building: "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device)<<"\n";
    exit(1);
    }
     
     
    // create buffers on the device
    cl::Buffer buffer_A(context,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_B(context,CL_MEM_READ_WRITE,sizeof(int)*10);
    cl::Buffer buffer_C(context,CL_MEM_READ_WRITE,sizeof(int)*10);
     
    int A[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int B[] = {0, 1, 2, 0, 1, 2, 0, 1, 2, 0};
     
    //create queue to which we will push commands for the device.
    cl::CommandQueue queue(context,default_device);
     
    //write arrays A and B to the device
    queue.enqueueWriteBuffer(buffer_A,CL_TRUE,0,sizeof(int)*10,A);
    queue.enqueueWriteBuffer(buffer_B,CL_TRUE,0,sizeof(int)*10,B);
     
     
    //run the kernel
    /*
    cl::KernelFunctor simple_add(cl::Kernel(program,"simple_add"),queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
    simple_add(buffer_A,buffer_B,buffer_C);
    */
    auto simple_add = cl::make_kernel<cl::Buffer &, cl::Buffer &, cl::Buffer &>(program,"simple_add");
cl::EnqueueArgs eargs(queue,cl::NullRange,cl::NDRange(10),cl::NullRange);
simple_add(eargs, buffer_A,buffer_B,buffer_C).wait(); 

    //alternative way to run the kernel
    /*cl::Kernel kernel_add=cl::Kernel(program,"simple_add");
    kernel_add.setArg(0,buffer_A);
    kernel_add.setArg(1,buffer_B);
    kernel_add.setArg(2,buffer_C);
    queue.enqueueNDRangeKernel(kernel_add,cl::NullRange,cl::NDRange(10),cl::NullRange);
    queue.finish();*/
     
    int C[10];
    //read result C from the device to array C
    queue.enqueueReadBuffer(buffer_C,CL_TRUE,0,sizeof(int)*10,C);
     
    std::cout<<" result: \n";
    for(int i=0;i<10;i++){
    std::cout<<C[i]<<" ";
    }
     
    return 0;
    }

    #else

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>
#undef CL_VERSION_1_2
#include <CL/cl.hpp>
#include <iostream>
#include <vector>

int main() {

    // Get the platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    // Loop over the number of platforms
    for ( size_t i = 0; i < platforms.size(); ++i ) {

        // Display the platform information
        std::cout << "Platform " << i+1 << ": "
                            << platforms[i].getInfo<CL_PLATFORM_NAME>()
        << "\n----------------------------------------------"
        << "\nVendor    : " << platforms[i].getInfo<CL_PLATFORM_VENDOR>()
        << "\nVersion   : " << platforms[i].getInfo<CL_PLATFORM_VERSION>();

        // Get the devices on the current platform
        std::vector <cl::Device> devices;
        platforms[i].getDevices( CL_DEVICE_TYPE_ALL , & devices);

        // Loop over the devices
        std::cout << "\n----------------------------------------------\n";
        for ( size_t j = 0; j < devices.size(); ++j ) {

            // Display the device information
            std::cout
            << "\n   Device " << j+1 << ": "
            <<          devices[j].getInfo< CL_DEVICE_NAME >()
            << "\n\t Device Version     : "
            <<          devices[j].getInfo< CL_DEVICE_VERSION >()
            << "\n\t OpenCL C Version   : "
            <<          devices[j].getInfo< CL_DEVICE_OPENCL_C_VERSION >()
            << "\n\t Compute Units      : "
            <<          devices[j].getInfo< CL_DEVICE_MAX_COMPUTE_UNITS >()
            << "\n\t Max Work Group Size: "
            <<          devices[j].getInfo< CL_DEVICE_MAX_WORK_GROUP_SIZE >()
            << "\n\t Clock Frequency    : "
            <<          devices[j].getInfo< CL_DEVICE_MAX_CLOCK_FREQUENCY >()
            << "\n\t Local Memory Size  : "
            <<          devices[j].getInfo< CL_DEVICE_LOCAL_MEM_SIZE >()
            << "\n\t Global Memory Size : "
            <<          devices[j].getInfo< CL_DEVICE_GLOBAL_MEM_SIZE >();

            // Check if the device supports double precision
            std::string str = devices[j].getInfo<CL_DEVICE_EXTENSIONS>();
            size_t found = str.find("cl_khr_fp64");
            std::cout << "\n\t Double Precision   : ";
            if ( found != std::string::npos ){ std::cout << "yes\n"; }
            else {                             std::cout <<  "no\n"; }
        }
        std::cout << "\n----------------------------------------------\n";
    }
//  std::cin.ignore();
    return 0;
}
    #endif
