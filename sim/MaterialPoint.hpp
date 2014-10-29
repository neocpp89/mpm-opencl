#ifndef MATERIALPOINT_HPP
#define MATERIALPOINT_HPP
#include <sstream>
#include <ios>
#include <string>
#include "Tensor.hpp"

#include <cstdio>

template <typename Real>
class Point2
{
    public:
        Real x, y;
        Point2() : x(0), y(0) { return; }
        Point2(const Real _x, const Real _y) : x(_x), y(_y) { return; }
        Point2(const Real *packed) : x(packed[0]), y(packed[1]) { return; }
};

template <typename Real>
class Point3
{
    public:
        Real x, y, z;
        Point3() : x(0), y(0), z(0) { return; }
        Point3(const Real _x, const Real _y, const Real _z) : x(_x), y(_y), z(_z) { return; }
        Point3(const Real *packed) : x(packed[0]), y(packed[1]), z(packed[2]) { return; }
};

/* Laziness.... */
template <typename Real>
using Vector2 = Point2<Real>;
template <typename Real>
using Vector3 = Point3<Real>;

template <typename Real>
class MaterialPoint3
{
    public:
        Point3<Real> Position;
        Vector3<Real> Velocity;
        Tensor<Real, 3> CauchyStress;
        
        MaterialPoint3() :
            Position(Point3<Real>(0,0,0)),
            Velocity(Vector3<Real>(0,0,0)),
            CauchyStress(Tensor<Real,3>(std::array<Real,9>({0,0,0,0,0,0,0,0,0})))
        {
            return;
        }

        static std::string getCSVHeaderString()
        {
            std::stringstream ss;
            ss << "x,y,z,";
            ss << "x_t,y_t,z_t,";
            ss << "Txx,Txy,Txz,Tyy,Tyz,Tzz";
            return ss.str();
        }

        std::string getCSVHexRowString()
        {
            std::stringstream ss;
            const size_t buflen = 64;
            char buf[buflen];
            std::snprintf(buf, buflen, "%la", Position.x);
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", Position.y);
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", Position.y);
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", Velocity.x);
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", Velocity.y);
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", Velocity.y);
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", CauchyStress(0,0));
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", CauchyStress(0,1));
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", CauchyStress(0,2));
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", CauchyStress(1,1));
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", CauchyStress(1,2));
            ss << buf << ",";
            std::snprintf(buf, buflen, "%la", CauchyStress(2,2));
            ss << buf << ",";
            return ss.str();
        }
 
        std::string getCSVRowString()
        {
            std::stringstream ss;
            // ss << std::hexfloat; //not implemented in g++...
            ss << Position.x << "," << Position.y << "," << Position.z << ",";
            ss << Velocity.x << "," << Velocity.y << "," << Velocity.z << ",";
            ss << CauchyStress(0,0) << "," << CauchyStress(0,1) << "," <<
                CauchyStress(0,2) << "," << CauchyStress(1,1) << "," <<
                CauchyStress(1,2) << "," << CauchyStress(2,2);
            return ss.str();
        }
};


/* template <typename Real = double, size_t dim = 3>
class Point
{
    private:
        std::array<Real, dim> coordinates;
    public:
        Point(const Real data[dim])
        {
            for (size_t i = 0; i < dim; i++) {
                coordinates[i] = data[i];
            }
            return;
        }
}; */

/* template <typename Real = double, size_t dim = 3>
class MaterialPoint
{
    private:
        Tensor<Real, dim> VelocityGradient;
        Tensor<Real, dim> CauchyStress;
}; */

#endif //MATERIALPOINT_HPP
