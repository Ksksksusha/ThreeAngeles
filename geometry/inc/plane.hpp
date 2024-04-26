#pragma once

#include "line.hpp"

namespace geometry {

enum mutual_pos
{
    INTERSECT,
    PARALLEL,
    EQUAL
};

class plane_t
{
    double a_ = NAN, b_ = NAN, c_ = NAN, d_ = NAN;

    vector_t norm_vec_;
    point_t  plane_pnt_;

    public:

    plane_t(const vector_t &norm_vec, const point_t &plane_pnt);

    bool is_valid() const { return norm_vec_.is_valid() && plane_pnt_.is_valid(); }

    mutual_pos get_mutual_pos_type(const plane_t &pln, const point_t &pnt) const;

    point_t get_line_intersection(const line_t &line) const;

    line_t get_intersection(const plane_t &plane) const;

    double calc_point(const point_t &pnt) const;

    void print() const;


    double get_a() const { return a_; }
    double get_b() const { return b_; }
    double get_c() const { return c_; }
    double get_d() const { return d_; }

    vector_t get_norm() const { return norm_vec_; }
};

}
