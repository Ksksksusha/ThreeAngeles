#pragma once

#include <stdexcept>

namespace geo_except {

struct point_non_valid_eq : std::runtime_error {
    point_non_valid_eq() : std::runtime_error("One of == points is non valid\n") {}
};

} // <-- geo_except