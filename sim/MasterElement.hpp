#ifndef MASTERELEMENT_HPP
#define MASTERELEMENT_HPP
#include <iterator>
#include <stdexcept>

namespace master_element {

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

/*
    This is the P1 (bilinear) Quad, which lives on [-1,1]x[-1,1] in xi, eta.
    The bottom left node is phi0, bottom right is phi1, top right phi2,
    and top left phi3.
*/
template <typename Real = double, bool check_writes = false>
class P1Quad
{
    using size_type = std::size_t;

    private:
        Real phi0(const Point2<Real> &p) const
        {
            return 0.25 * (1.0 - p.x) * (1.0 - p.y);
        }

        Real phi1(const Point2<Real> &p) const
        {
            return 0.25 * (1.0 + p.x) * (1.0 - p.y);
        }

        Real phi2(const Point2<Real> &p) const
        {
            return 0.25 * (1.0 + p.x) * (1.0 + p.y);
        }

        Real phi3(const Point2<Real> &p) const
        {
            return 0.25 * (1.0 - p.x) * (1.0 + p.y);
        }

    public:
        template <typename Iterator>
        void phi(const Point2<Real> &p, Iterator begin, Iterator end) const
        {
            if (check_writes) {
                std::size_t dist = std::distance(begin, end);
                if (dist < getNumberOfNodes()) {
                    throw std::runtime_error("Output not large enough.");
                }
            }

            *begin = phi0(p);
            begin++;
            *begin = phi1(p);
            begin++;
            *begin = phi2(p);
            begin++;
            *begin = phi3(p);
            begin++;

            return;
        }

        size_type getNumberOfNodes() const { return 4; }
};

} // namespace master_element

#endif //MASTERELEMENT_HPP
