#pragma once

#include "point.hpp"
#include <iostream>


namespace geometry {

class vector_t
{
    double x_, y_, z_;

    public:

    vector_t(double x = NAN, double y = NAN, double z = NAN) : x_(x), y_(y), z_(z) {}
    vector_t(const point_t  &pnt) : x_(pnt.get_x()), y_(pnt.get_y()), z_(pnt.get_z()) {}

    bool is_valid() const { return (std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_)); }


    bool operator== (const vector_t &vec2) const;
    bool operator!= (const vector_t &vec2) const;

    vector_t operator- (const vector_t &vec2) const;
    vector_t operator+ (const vector_t &vec2) const;

    vector_t vec_product(const vector_t &vec2) const;

    double sqal_product(const vector_t &vec2) const;
    double get_squared_len() const;

    vector_t normalized() const;
    vector_t negative() const { return {-x_, -y_, -z_}; }

    void   print() const { std::cout << "(" << x_ << ", " << y_ << ", " << z_ << ")" << std::endl; }
    double get_x() const { return x_; }
    double get_y() const { return y_; }
    double get_z() const { return z_; }
};

const vector_t NULL_VEC = vector_t{0, 0, 0};
const vector_t NAN_VEC  = vector_t{NAN, NAN, NAN};

}
