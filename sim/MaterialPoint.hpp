#ifndef MATERIALPOINT_HPP
#define MATERIALPOINT_HPP
#include <sstream>
#include <ios>
#include <string>
#include <map>

#include <cstdio>

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

/* Laziness.... */
template <typename Real>
using Vector2 = Point2<Real>;
template <typename Real>
using Vector3 = Point3<Real>;

template <typename Real = double>
class MaterialPoint3
{
    public:
        size_t id;
        Point3<Real> Position;
        Vector3<Real> Velocity;
        Tensor<Real, 3> CauchyStress;
        Tensor<Real, 3> TimeAveragedCauchyStress;
        std::map<std::string, Real> StateVariables;
        
        MaterialPoint3(const size_t _id = 0) :
            id(_id),
            Position({0,0,0}),
            Velocity({0,0,0}),
            CauchyStress(Tensor<Real, 3>(0)),
            TimeAveragedCauchyStress(Tensor<Real, 3>(0))
        {
            return;
        }

        std::string getCSVHeaderString() const
        {
            std::stringstream ss;
            ss << "id,";
            ss << "x,y,z,";
            ss << "x_t,y_t,z_t,";
            ss << "Txx,Txy,Txz,Tyy,Tyz,Tzz,";
            ss << "Txx_bar,Txy_bar,Txz_bar,Tyy_bar,Tyz_bar,Tzz_bar";

            // Use the keys as the header names.
            for (auto const &k : StateVariables) {
                ss << ',' << k.first;
            }

            return ss.str();
        }

        /*
            \brief Function to write data as a row of hex doubles.

            I would have just passed std::hexfloat to getCSVRowString,
            however it is not implemented in libstdc++/g++ yet (Oct 2014).
        */
        std::string getCSVHexRowString() const
        {
            std::stringstream ss;
            const size_t buflen = 64;
            char buf[buflen];

            ss << id << ',';

            std::snprintf(buf, buflen, "%la", Position.x);
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", Position.y);
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", Position.z);
            ss << buf << ',';

            std::snprintf(buf, buflen, "%la", Velocity.x);
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", Velocity.y);
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", Velocity.z);
            ss << buf << ',';

            std::snprintf(buf, buflen, "%la", CauchyStress(0,0));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", CauchyStress(0,1));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", CauchyStress(0,2));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", CauchyStress(1,1));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", CauchyStress(1,2));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", CauchyStress(2,2));
            ss << buf << ',';

            std::snprintf(buf, buflen, "%la", TimeAveragedCauchyStress(0,0));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", TimeAveragedCauchyStress(0,1));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", TimeAveragedCauchyStress(0,2));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", TimeAveragedCauchyStress(1,1));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", TimeAveragedCauchyStress(1,2));
            ss << buf << ',';
            std::snprintf(buf, buflen, "%la", TimeAveragedCauchyStress(2,2));
            ss << buf;

            // Write state variables as well.
            for (auto const &k : StateVariables) {
                std::snprintf(buf, buflen, "%la", k.second);
                ss << ',' << buf;
            }

            return ss.str();
        }
 
        std::string getCSVRowString(std::ios_base &(format)(std::ios_base &) = std::scientific) const
        {
            std::stringstream ss;
            // ss << std::hexfloat; //not implemented in libstdc++/g++...
            if (format != nullptr) {
                ss << format;
            }
            ss << id << ','; 
            ss << Position.x << ',' << Position.y << ',' << Position.z << ',';
            ss << Velocity.x << ',' << Velocity.y << ',' << Velocity.z << ',';
            ss << CauchyStress(0,0) << ','
                << CauchyStress(0,1) << ','
                << CauchyStress(0,2) << ','
                << CauchyStress(1,1) << ','
                << CauchyStress(1,2) << ','
                << CauchyStress(2,2) << ',';
            ss << TimeAveragedCauchyStress(0,0) << ','
                << TimeAveragedCauchyStress(0,1) << ','
                << TimeAveragedCauchyStress(0,2) << ','
                << TimeAveragedCauchyStress(1,1) << ','
                << TimeAveragedCauchyStress(1,2) << ','
                << TimeAveragedCauchyStress(2,2);
            
            for (auto const &k : StateVariables) {
                ss << ',' << k.second;
            }

            return ss.str();
        }
};

#endif //MATERIALPOINT_HPP
