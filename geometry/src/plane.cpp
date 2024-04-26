#include "double_operations.hpp"
#include "custom_assert.hpp"
#include "plane.hpp"
#include "line.hpp"
#include <iostream>

using namespace geometry;
using namespace doperations;


plane_t::plane_t(const vector_t &norm_vec, const point_t &plane_pnt) : norm_vec_(norm_vec), plane_pnt_(plane_pnt)
{
    a_ = norm_vec.get_x();
    b_ = norm_vec.get_y();
    c_ = norm_vec.get_z();

    d_ = -1 * (a_ * plane_pnt.get_x() + b_ * plane_pnt.get_y() + c_ * plane_pnt.get_z());
}


mutual_pos plane_t::get_mutual_pos_type(const plane_t &pln, const point_t &pnt) const
{
    ASSERT(is_valid());
    ASSERT(pln.is_valid());
    ASSERT(pnt.is_valid());

    vector_t res_vec = norm_vec_.vec_product(pln.norm_vec_);

    if (res_vec != NULL_VEC) return INTERSECT;

    if (is_equal(pln.a_ * pnt.get_x() + pln.b_ * pnt.get_y() + pln.c_ * pnt.get_z() + pln.d_, 0))
        return EQUAL;

    return PARALLEL;
}


void plane_t::print() const
{
    std::cout << "PLANE\nnorm vec = ";

    norm_vec_.print();

    std::cout << "point = ";
    plane_pnt_.print();
}


line_t plane_t::get_intersection(const plane_t &plane2) const
{
    ASSERT(is_valid());
    ASSERT(plane2.is_valid());

    double main_det = b_ * plane2.c_ - plane2.b_ * c_;
    if (!is_equal(main_det, 0))
    {
        double sub_det1 = plane2.d_ * c_ - d_ * plane2.c_;
        double sub_det2 = plane2.b_ * d_ - b_ * plane2.d_;

        return line_t{{norm_vec_.vec_product(plane2.norm_vec_)}, {0, (sub_det1 / main_det), (sub_det2 / main_det)}};
    }

    main_det = a_ * plane2.c_ - plane2.a_ * c_;
    if (!is_equal(main_det, 0))
    {
        double sub_det1 = plane2.d_ * c_ - d_ * plane2.c_;
        double sub_det2 = plane2.a_ * d_ - a_ * plane2.d_;

        return line_t{{norm_vec_.vec_product(plane2.norm_vec_)}, {(sub_det1 / main_det), 0, (sub_det2 / main_det)}};
    }

    main_det = a_ * plane2.b_ - plane2.a_ * b_;
    if(!is_equal(main_det, 0))
    {
        double sub_det1 = plane2.d_ * b_ - d_ * plane2.b_;
        double sub_det2 = plane2.a_ * d_ - a_ * plane2.d_;

        return line_t{{norm_vec_.vec_product(plane2.norm_vec_)}, {(sub_det1 / main_det), (sub_det2 / main_det), 0}};
    }

    return {NAN_VEC, NAN_PNT};
}


double plane_t::calc_point(const point_t &pnt) const
{
    ASSERT(is_valid());
    ASSERT(pnt.is_valid());

    return a_ * pnt.get_x() + b_ * pnt.get_y() + c_ * pnt.get_z() + d_;
}


point_t plane_t::get_line_intersection(const line_t &line) const
{
    ASSERT(is_valid());
    ASSERT(line.is_valid());

    if (is_equal(norm_vec_.sqal_product(line.get_dir_vec()), 0))
    {
        if (is_equal(calc_point(line.get_line_pnt()), 0)) return SPEC_PNT;
        return NAN_PNT;
    }

    vector_t dir_vec = line.get_dir_vec();
    point_t pnt = line.get_line_pnt();

    double denom = a_ * dir_vec.get_x() + b_ * dir_vec.get_y() + c_ * dir_vec.get_z();
    double t0 = -1 * calc_point(pnt) / denom;


    return {pnt.get_x() + dir_vec.get_x() * t0, pnt.get_y() + dir_vec.get_y() * t0, pnt.get_z() + dir_vec.get_z() * t0};
}