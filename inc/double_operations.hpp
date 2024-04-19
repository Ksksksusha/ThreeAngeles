#pragma once

#include <cmath>

namespace doperations {

const double ACCURACY = 0.00001;

inline bool is_equal(double num1,  double num2) { return (std::abs(num1 - num2) < ACCURACY); }

inline double triple_min(double num1, double num2, double num3) { return std::min(std::min(num1, num2), num3); }
inline double triple_max(double num1, double num2, double num3) { return std::max(std::max(num1, num2), num3); }

inline bool gr_or_eq(double num1, double num2) { return num1 > num2 || is_equal(num1, num2); }
inline bool ls_or_eq(double num1, double num2) { return num1 < num2 || is_equal(num1, num2); }

}
