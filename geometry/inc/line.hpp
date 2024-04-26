#pragma once

#include "vector.hpp"

namespace geometry {

class line_t
{
    vector_t dir_vec_;
    point_t  line_pnt_;

    double p1_, p2_, p3_;

    public:

    line_t(const vector_t &dir_vec, const point_t &line_pnt) : dir_vec_(dir_vec), line_pnt_(line_pnt), p1_(dir_vec_.get_x()), p2_(dir_vec_.get_y()), p3_(dir_vec_.get_z()) {}


    bool operator== (const line_t &line2) const;

    bool check_point_belong(const point_t &pnt) const;

    point_t  get_line_intersection(const line_t &line2) const;

    point_t  get_line_pnt() const { return line_pnt_; }

    vector_t get_dir_vec() const { return dir_vec_; }

    bool is_valid() const { return dir_vec_.is_valid() && line_pnt_.is_valid(); }


    void print() const;
};

}
