#include "double_operations.hpp"
#include "custom_assert.hpp"
#include "line.hpp"
#include "plane.hpp"
#include <iostream>
#include <cmath>

using namespace geometry;
using namespace doperations;


bool line_t::operator== (const line_t &line2) const
{
    return (dir_vec_.vec_product(line2.dir_vec_) == NULL_VEC) && check_point_belong(line2.line_pnt_);
}


bool line_t::check_point_belong(const point_t &pnt) const
{
    ASSERT(is_valid());
    ASSERT(pnt.is_valid());

    double t0 = 0;
    if (!is_equal(p1_, 0))
        t0 = (pnt.get_x() - line_pnt_.get_x()) / p1_;

    if (!is_equal(p2_, 0))
        t0 = (pnt.get_y() - line_pnt_.get_y()) / p2_;

    if (!is_equal(p3_, 0))
        t0 = (pnt.get_z() - line_pnt_.get_z()) / p3_;

    return is_equal(pnt.get_x() - line_pnt_.get_x(), t0 * p1_) &&
           is_equal(pnt.get_y() - line_pnt_.get_y(), t0 * p2_) &&
           is_equal(pnt.get_z() - line_pnt_.get_z(), t0 * p3_);
}


point_t line_t::get_line_intersection(const line_t &line2) const
{
    ASSERT(is_valid());
    ASSERT(line2.is_valid());

    if ((dir_vec_.vec_product(line2.dir_vec_) == NULL_VEC))
    {
        if (check_point_belong(line2.line_pnt_)) return SPEC_PNT;
        return NAN_PNT;
    }

    double main_det = p2_ * line2.p1_ - p1_ * line2.p2_;
    if (!is_equal(main_det, 0))
    {
        double sub_det = (line2.line_pnt_.get_y() - line_pnt_.get_y()) * line2.p1_ -
                         (line2.line_pnt_.get_x() - line_pnt_.get_x()) * line2.p2_;

        double t0 = (sub_det / main_det);
        point_t pnt{line_pnt_.get_x() + t0 * p1_,
                    line_pnt_.get_y() + t0 * p2_,
                    line_pnt_.get_z() + t0 * p3_};

        if (line2.check_point_belong(pnt)) return pnt;
        return NAN_PNT;
    }

    main_det = p3_ * line2.p1_ - p1_ * line2.p3_;
    if (!is_equal(main_det, 0))
    {
        double sub_det = (line2.line_pnt_.get_z() - line_pnt_.get_z()) * line2.p1_ -
                         (line2.line_pnt_.get_x() - line_pnt_.get_x()) * line2.p3_;

        double t0 = (sub_det / main_det);
        point_t pnt{line_pnt_.get_x() + t0 * p1_,
                    line_pnt_.get_y() + t0 * p2_,
                    line_pnt_.get_z() + t0 * p3_};

        if (line2.check_point_belong(pnt)) return pnt;
        return NAN_PNT;
    }

    main_det = p3_ * line2.p2_ - p2_ * line2.p3_;
    double sub_det = (line2.line_pnt_.get_z() - line_pnt_.get_z()) * line2.p2_ -
                     (line2.line_pnt_.get_y() - line_pnt_.get_y()) * line2.p3_;

    double t0 = (sub_det / main_det);
    point_t pnt{line_pnt_.get_x() + t0 * p1_,
                line_pnt_.get_y() + t0 * p2_,
                line_pnt_.get_z() + t0 * p3_};

    if (line2.check_point_belong(pnt)) return pnt;
    return NAN_PNT;
}


void line_t::print() const
{
    std::cout << std::endl << "line vec: ";
    dir_vec_.print();
    std::cout << "line pnt: ";
    line_pnt_.print();
}
