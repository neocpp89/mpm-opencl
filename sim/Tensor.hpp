#ifndef TENSOR_HPP
#define TENSOR_HPP
#include <array>
#include <exception>
#include <cmath>
#include <utility>

#include <iostream>

template <typename Real, size_t dimension>
class Tensor
{
    private:
        std::array<Real, dimension*dimension> data;

        static size_t linearIndex(const size_t i, const size_t j) { return (i * dimension + j); }

    public:
        Tensor() { return; }
        Tensor(const std::array<Real, dimension*dimension> &A)
        {
            for (size_t i = 0; i < dimension; i++) {
                for (size_t j = 0; j < dimension; j++) {
                    data[linearIndex(i, j)] = A[linearIndex(i, j)];
                }
            }

            return;
        }
        Tensor(const Tensor &A)
        {
            for (size_t i = 0; i < A.dim(); i++) {
                for (size_t j = 0; j < A.dim(); j++) {
                    data[linearIndex(i, j)] = A(i, j);
                }
            }
            return;
        }
        ~Tensor() { return; }

        static size_t dim() { return dimension; }

        Real trace() const
        {
            Real tr = 0;
            for (size_t i = 0; i < dimension; i++) {
                tr += data[linearIndex(i, i)];
            }
            return tr;
        }

        void decompose(Tensor &deviator, Real &spherical) const
        {
            spherical = this->trace() / (Real)dimension;
            for (size_t i = 0; i < dimension; i++) {
                for (size_t j = 0; j < dimension; j++) {
                    deviator(i, j) = data[linearIndex(i, j)];
                }
                deviator(i, i) -= spherical;
            }
        }

        Tensor deviator() const
        {
            Tensor dev;
            Real tr;
            this->decompose(dev, tr);
            return dev;
        }

        Real contraction(const Tensor &A) const
        {
            Real s = 0;

            for (size_t i = 0; i < dimension; i++) {
                for (size_t j = 0; j < dimension; j++) {
                    s += A(i, j) * data[linearIndex(i, j)];
                }
            }

            return s;
        }

        Real frobeniusNorm() const
        {
            return std::sqrt(contraction(*this));
        }

        Real & operator()(const size_t i, const size_t j) { return data[linearIndex(i, j)]; }
        Real operator()(const size_t i, const size_t j) const { return data[linearIndex(i, j)]; }

        void swap(Tensor &A)
        {
            std::swap(this->data, A.data);
            return;
        }

        Tensor & operator=(Tensor A)
        {
            A.swap(*this);
            return (*this);
        }

        inline bool operator==(const Tensor &rhs) const
        {
            if (dimension != rhs.dim()) {
                return false;
            }

            for (size_t i = 0; i < dimension; i++) {
                for (size_t j = 0; j < dimension; j++) {
                    if (data[linearIndex(i, j)] != rhs(i, j)) {
                        return false;
                    }
                }
            }

            return true;
        }
        inline bool operator!=(const Tensor &rhs) const
        {
            return !operator==(rhs);
        }

        friend std::ostream& operator<<(std::ostream& os, const Tensor& A)
        {
            os << "\n[\n";
            for (size_t i = 0; i < A.dim(); i++) {
                os << "  ";
                for (size_t j = 0; j < A.dim(); j++) {
                    os << A(i,j) << " ";
                }
                os << "\n";
            }
            os << "]";
            return os;
        }
};

#endif //TENSOR_HPP
