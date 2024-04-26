#include "segment.hpp"
#include "plane.hpp"
#include "custom_assert.hpp"
#include "double_operations.hpp"

using namespace geometry;
using namespace doperations;


bool segment_t::contains_inter_pnt(const point_t &pnt) const
{
    double x_min = std::min(first_.get_x(), second_.get_x()),
           x_max = std::max(first_.get_x(), second_.get_x()),
           y_min = std::min(first_.get_y(), second_.get_y()),
           y_max = std::max(first_.get_y(), second_.get_y()),
           z_min = std::min(first_.get_z(), second_.get_z()),
           z_max = std::max(first_.get_z(), second_.get_z());

    return gr_or_eq(pnt.get_x(), x_min) && ls_or_eq(pnt.get_x(), x_max) &&
           gr_or_eq(pnt.get_y(), y_min) && ls_or_eq(pnt.get_y(), y_max) &&
           gr_or_eq(pnt.get_z(), z_min) && ls_or_eq(pnt.get_z(), z_max);
}


bool segment_t::intersects_seg(const segment_t &seg2) const
{
    line_t seg_line1{vector_t{second_} - vector_t{first_}, first_};
    line_t seg_line2{vector_t{seg2.second_} - vector_t{seg2.first_}, seg2.first_};

    point_t intersection_pnt{seg_line1.get_line_intersection(seg_line2)};

    if (intersection_pnt.special_check())
    {
        if (contains_inter_seg(seg2)) return true;
        return false;
    }

    return contains_inter_pnt(intersection_pnt) &&
      seg2.contains_inter_pnt(intersection_pnt);
}


point_t segment_t::get_line_intersection(const line_t &line) const
{
    if ((dir_vec_.vec_product(line.get_dir_vec()) == NULL_VEC) && !(line.check_point_belong(first_)))
        return NAN_PNT;

    if ((dir_vec_.vec_product(line.get_dir_vec()) == NULL_VEC) && (line.check_point_belong(first_)))
        return SPEC_PNT;

    point_t intersection_pnt{seg_line_.get_line_intersection(line)};

    if (contains_inter_pnt(intersection_pnt)) return intersection_pnt;
    return NAN_PNT;
}


bool segment_t::contains_inter_seg(const segment_t &seg2) const
{
    return contains_inter_pnt(seg2.first_) || contains_inter_pnt(seg2.second_) ||
           seg2.contains_inter_pnt(first_) || seg2.contains_inter_pnt(second_);
}


point_t segment_t::get_plane_intersection(const plane_t &pln) const
{
    ASSERT(is_valid());
    ASSERT(pln.is_valid());

    point_t pnt = pln.get_line_intersection(seg_line_);
    if (!pnt.is_valid()) return pnt;

    if (!contains_inter_pnt(pnt)) return NAN_PNT;
    return pnt;
}


void segment_t::print() const
{
    std::cout << "segment:\n";
    first_.print();
    second_.print();
}
