#ifndef MESH_HPP
#define MESH_HPP
#include "MasterElement.hpp"
#include "Tensor.hpp"

template <typename Real>
struct Point2
{
    Real x, y;
};

template <typename Real>
struct Point3
{
    Real x, y, z;
};

template <typename Real>
struct P1QuadPoints
{
    Point2<Real> points[4];
};

/* Laziness.... */
template <typename Real>
using Vector2 = Point2<Real>;
template <typename Real>
using Vector3 = Point3<Real>;

template <typename Real = double>
class StructuredFixedAR2D
{
    using size_type = std::size_t;

    private:
    Real aspect; // calculated as height over width of each element
    const Point2<Real> upper_left;
    const Point2<Real> lower_right;
    const size_type Nx; // number of elements in x-direction
    const size_type Ny;

    const master_element::P1Quad<Real> master;
    

    public:
    StructuredFixedAR2D(const Point2<Real> &upper_left_, const Point2<Real> &lower_right_, const size_type Nx_, const size_type Ny_)
        : upper_left(upper_left_), lower_right(lower_right_), Nx(Nx_), Ny(Ny_), master()
    {
        const Real Lx = lower_right.x - upper_left.x;
        const Real Ly = upper_left.y - lower_right.y;
        aspect = (Ly*Nx) / (Lx*Ny);
    }

    size_type getNumberOfElements() const
    {
        return Nx*Ny;
    }

};

#endif //MESH_HPP
