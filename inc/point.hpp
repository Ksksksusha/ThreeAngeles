#pragma once

#include <cmath>
#include <iostream>
#include "custom_assert.hpp"
#include "double_operations.hpp"

namespace geometry {

class point_t
{
    double x_, y_, z_;

    public:

    point_t(double x = NAN, double y = NAN, double z = NAN) : x_(x), y_(y), z_(z) {}

    bool is_valid() const { return (std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_)); }

    void print() const { std::cout << "(" << x_ << ", " << y_ << ", " << z_ << ")" << std::endl; }

    double get_x() const { return x_; }
    double get_y() const { return y_; }
    double get_z() const { return z_; }
};

const point_t NAN_PNT  = {NAN, NAN, NAN};
const point_t SPEC_PNT = {NAN, 0, 0};
const point_t NULL_PNT = {0, 0, 0};

}
