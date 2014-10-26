// We need doubles...
#pragma OPENCL EXTENSION cl_khr_fp64: enable

inline double2 vecfrompts(double2 to, double2 from)
{
    double2 r;
    r.x = to.x - from.x;
    r.y = to.y - from.y;
    return r;
}

__kernel
void tri2d_local_coordinates(
    ulong num_particles,
    __global double2* p,
    __global double2* a,
    __global double2* b,
    __global double2* c,
    __global double4* sf)
{
    const size_t i = get_global_id(0);
    
    if (i < num_particles) {
        // from Christer Ericson (Real-Time Collision Detection)
        const double2 v0 = vecfrompts(b[i], a[i]);
        const double2 v1 = vecfrompts(c[i], a[i]);
        const double2 v2 = vecfrompts(p[i], a[i]);
        const double d00 = dot(v0, v0);
        const double d01 = dot(v0, v1);
        const double d11 = dot(v1, v1);
        const double d20 = dot(v2, v0);
        const double d21 = dot(v2, v1);
        const double denom = d00 * d11 - d01 * d01;
        if (denom > 0) {
            const double v = (d11 * d20 - d01 * d21) / denom;
            const double w = (d00 * d21 - d01 * d20) / denom;
            const double u = 1.0 - v - w;
            sf[i] = (double4)(u, v, w, 0);
        } else {
            sf[i] = (double4)(0, 0, 0, 0);
        }
    }
    return;
}
