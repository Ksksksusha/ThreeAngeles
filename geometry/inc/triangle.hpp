#pragma once

#include "segment.hpp"
#include "plane.hpp"


namespace geometry {

const double brad_coeff = 18; // koeff that is used in bounding radius in triangle_t::intesects() = 3^2 * 2 (2 cuz of 2*(a^2 + b^2) > (a+b)^2)

enum triag_type
{
    TRIAG,
    SEGMENT,
    POINT
};

class triangle_t
{
    point_t   A_, B_, C_;
    triag_type type_;

    plane_t pln_{(vector_t{B_} - vector_t{A_}).vec_product(vector_t{C_} - vector_t{A_}).normalized(), A_};

    vector_t center_x3_;
    double bounding_rad_sq_ = NAN;


    bool is_in_triag(const point_t &pnt) const;

    bool check_triags_in_intersect_planes(const triangle_t &triag2) const;

    bool check_triags_in_same_plane(const triangle_t &triag2) const;

    segment_t get_triag_intersection(const line_t &line) const;

    segment_t get_triag_seg() const;

    vector_t get_center_vec_x3() const;

    triag_type get_triag_type() const;


    bool intersects_triag_triag     (const triangle_t &triag2) const;
    bool intersects_triag_segment   (const triangle_t &triag2) const;
    bool intersects_triag_point     (const triangle_t &triag2) const;

    bool intersects_segment_segment (const triangle_t &triag2) const;
    bool intersects_segment_point   (const triangle_t &triag2) const;
    bool intersects_point_point     (const triangle_t &triag2) const;

    bool check_triag_intersect_plane(const triangle_t &triag2) const;


    public:

    triangle_t(const point_t &A, const point_t &B, const point_t &C);

    segment_t get_segment() const;

    bool is_valid() const { return (A_.is_valid() && B_.is_valid() && C_.is_valid()); }

    bool intersects(const triangle_t &triag2) const;

    void print() const;

    point_t getA() const { return A_; }
    point_t getB() const { return B_; }
    point_t getC() const { return C_; }

    plane_t get_plane() const { return pln_; }
};

}
