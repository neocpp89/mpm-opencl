// We need doubles...
#pragma OPENCL EXTENSION cl_khr_fp64: enable

inline double4 integrate_particle_positions(
    double4 particle_velocity,
    double4 particle_position,
    double dt
)
{
    double4 dtv = (double4)(dt, dt, dt, dt);
    return fma(particle_velocity, dtv, particle_position);
}

inline double2 vec2frompts(double2 to, double2 from)
{
    double2 r;
    r.x = to.x - from.x;
    r.y = to.y - from.y;
    return r;
}

inline double4 vec4frompts(double4 to, double4 from)
{
    double4 r;
    r.x = to.x - from.x;
    r.y = to.y - from.y;
    r.z = to.z - from.z;
    r.w = to.w - from.w;
    return r;
}

inline double4 tri2d_local_from_global(
    double2 global_coords,
    double2 A,
    double2 B,
    double2 C)
{
    double4 local_coords;

    // from Christer Ericson (Real-Time Collision Detection)
    const double2 v0 = vec2frompts(B, A);
    const double2 v1 = vec2frompts(C, A);
    const double2 v2 = vec2frompts(global_coords, A);
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
        local_coords = (double4)(u, v, w, 0);
    } else {
        local_coords = (double4)(0, 0, 0, 0);
    }

    return local_coords;
}

inline double4 tri2d_shape_functions(
    double4 local_coords,
    double2 A,
    double2 B,
    double2 C
)
{
    // for triangular elements, the local coordinates are the shape functions.
    return local_coords;
}
inline double8 tri2d_gradient_shape_functions(
    double4 local_coords,
    double2 A,
    double2 B,
    double2 C
)
{
    const double detJ = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
    double4 ddx = (B.y - C.y, C.y - A.y, A.y - B.y, 0);
    double4 ddy = (C.x - B.x, A.x - C.x, B.x - A.x, 0);
    ddx = ddx / detJ;
    ddy = ddy / detJ;
    // low parts are d/dx, high parts are d/dy
    const double8 dsf = (double8)(ddx, ddy);
    return dsf;
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
        sf[i] = tri2d_local_from_global(p[i], a[i], b[i], c[i]);
    }
    return;
}

__kernel
void tri2d_local_coordinates_from_nodelist(
    ulong num_particles,
    __global double2* global_coordinates,
    __global ulong4* node_ids_from_element_map,
    __global ulong* element_id_from_particle_map,
    __global double2* node_coordinates,
    __global double4* local_coordinates)
{
    const ulong i = get_global_id(0);

    if (i < num_particles) {
        const ulong element_id = element_id_from_particle_map[i];
        const ulong4 node_ids = node_ids_from_element_map[element_id];
        const ulong A_id = node_ids.x;
        const ulong B_id = node_ids.y;
        const ulong C_id = node_ids.z;
        const double2 p = global_coordinates[i];
        const double2 a = node_coordinates[A_id];
        const double2 b = node_coordinates[B_id];
        const double2 c = node_coordinates[C_id];
        local_coordinates[i] = tri2d_local_from_global(p, a, b, c);
    }
    return;
}

__kernel
void tri2d_sort_into_elements_full(
    ulong num_particles,
    ulong num_elements,
    __global double2* global_coordinates,
    __global ulong4* node_ids_from_element_map,
    __global ulong* element_id_from_particle_map,
    __global double2* node_coordinates,
    __global double4* local_coordinates,
    __global uchar* active)
{
    const ulong i = get_global_id(0);
    if (i < num_particles && active[i] != 0) {
        active[i] = 0;
        for (ulong j = 0; j < num_elements; j++) {
            const ulong4 node_ids = node_ids_from_element_map[j];
            const ulong A_id = node_ids.x;
            const ulong B_id = node_ids.y;
            const ulong C_id = node_ids.z;
            const double2 p = global_coordinates[i];
            const double2 a = node_coordinates[A_id];
            const double2 b = node_coordinates[B_id];
            const double2 c = node_coordinates[C_id];
            const double4 sf = tri2d_local_from_global(p, a, b, c);
            if (sf.x >= 0 && sf.x <= 1 &&
                sf.y >= 0 && sf.y <= 1 &&
                sf.z >= 0 && sf.z <= 1) {
                // point is inside the triangle
                local_coordinates[i] = sf;
                active[i] = 1;
                // go ahead and calculate the shape functions
                // and shape function gradients too
                tri2d_shape_functions(sf, a, b, c);
                tri2d_gradient_shape_functions(sf, a, b, c);
                break;
            }
        }
    }
    return;
}
