#ifndef MATRIX_HPP 
#define MATRIX_HPP
#include <array>
template <typename Real, size_t dimension>
class Tensor
{
    private:
        std::array<Real, dimension*dimension> data;
    
    public:

        Real & operator()(size_t i, size_t j) { return data[i * dimension + j]; }
        Real operator()(size_t i, size_t j) const { return data[i * dimension + j]; }
        size_t dim() { return dimension; }
};

#endif //MATRIX_HPP
