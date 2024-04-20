#pragma once

#include <iostream>
#include "double_op.hpp"
#include "geo_except.hpp"

namespace geometry {

using namespace double_op;
using namespace geo_except;

class point_t {
    double x_, y_, z_;

public:
    point_t(double x = NAN, double y = NAN, double z = NAN) : x_(x), y_(y), z_(z) {}

    bool is_valid() const { return (std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_)); }

    bool operator== (const point_t &pnt) const {
        if (!is_valid() || !pnt.is_valid())
            throw point_non_valid_eq();

        return (is_equal(x_, pnt.x_) && is_equal(y_, pnt.y_) && is_equal(z_, pnt.z_));
    }

    bool operator!= (const point_t &pnt) const { return !(*this == pnt); }

    void print() const { std::cout << "(" << x_ << ", " << y_ << ", " << z_ << ")" << std::endl; }

    double get_x() const { return x_; }
    double get_y() const { return y_; }
    double get_z() const { return z_; }
};

const point_t NAN_PNT  = {NAN, NAN, NAN};
const point_t SPEC_PNT = {NAN, 0, 0};
const point_t NULL_PNT = {0, 0, 0};

} // <-- geometry