mpm-opencl
==========

`mpm-opencl` is an implemention of the Material Point Method using OpenCL.
This is research code, so it is not feature complete for general use.

## Build and Install
--------------------
`mpm-opencl` uses [cmake][0] for the build process. Run the following in the 
terminal at the top-level directory:

    mkdir build
    cd build
    cmake ..
    make

This results in the executable `mpm-opencl` in the build directory.

[0]: http://www.cmake.org/

## Licensing
------------
Some components and libaries are required. OpenCL is obviously used as well,
but has no licensing restriction.:

    * [gzstream][1] (LGPL 2.1)
    * [The Lean Mean C++ Option Parser][2] (MIT license)
    * [zlib][3] (compatible with GPL)

[1]: https://gist.github.com/piti118/1508048
[2]: http://optionparser.sourceforge.net/
[3]: http://www.zlib.net/

