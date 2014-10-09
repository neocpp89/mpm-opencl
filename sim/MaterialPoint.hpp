#ifndef MATERIALPOINT_HPP
#define MATERIALPOINT_HPP
#include "Tensor.hpp"

template <typename Real = double, size_t dim = 3>
class MaterialPoint
{
    Tensor<Real, dim> VelocityGradient;
    Tensor<Real, dim> CauchyStress;
};

#endif //MATERIALPOINT_HPP
