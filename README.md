mpm-opencl
==========

`mpm-opencl` is an implemention of the Material Point Method using OpenCL.
This is research code, so it is not feature complete for general use.

## Build and Install
--------------------
`mpm-opencl` uses `cmake` for the build process. Run the following in the 
terminal at the top-level directory:

    mkdir build
    cd build
    cmake ..
    make

This results in the executable `mpm-opencl` in the build directory.

## Licensing
------------
Some components and libaries are required:
    + ZLIB (compatible with GPL)
    + gzstream (LGPL 2.1)

