#pragma once

#include <cmath>

namespace double_op {
    constexpr double ACCURACY = 0.00001;

    inline bool is_equal(double num1,  double num2) { return (std::abs(num1 - num2) < ACCURACY); }

    inline double triple_min(double num1, double num2, double num3) { return std::min(std::min(num1, num2), num3); }
    inline double triple_max(double num1, double num2, double num3) { return std::max(std::max(num1, num2), num3); }

    inline bool gr_or_eq(double num1, double num2) { return num1 > num2 || is_equal(num1, num2); }
    inline bool ls_or_eq(double num1, double num2) { return num1 < num2 || is_equal(num1, num2); }

    inline bool all_positive(double num1, double num2, double num3) {
        if (is_equal(num1, 0)) num1 = 0;
        if (is_equal(num2, 0)) num2 = 0;
        if (is_equal(num3, 0)) num3 = 0;

        return (num1 > 0) && (num2 > 0) && (num3 > 0);
    }

    inline bool all_negative(double num1, double num2, double num3) {
        if (is_equal(num1, 0)) num1 = 0;
        if (is_equal(num2, 0)) num2 = 0;
        if (is_equal(num3, 0)) num3 = 0;

        return (num1 < 0) && (num2 < 0) && (num3 < 0);
    }

    inline bool all_positive_triag_special(double num1, double num2, double num3) {
        if (is_equal(num1, 0)) num1 = 0;
        if (is_equal(num2, 0)) num2 = 0;
        if (is_equal(num3, 0)) num3 = 0;

        if (num1 < 0) return false;
        if (num2 < 0) return false;
        if (num3 < 0) return false;

        int zeros = 0;

        if (is_equal(num1, 0)) ++zeros;
        if (is_equal(num2, 0)) ++zeros;
        if (is_equal(num3, 0)) ++zeros;

        if (zeros > 2) return false;
        return true;
    }

    inline bool all_negative_triag_special(double num1, double num2, double num3) {
        if (is_equal(num1, 0)) num1 = 0;
        if (is_equal(num2, 0)) num2 = 0;
        if (is_equal(num3, 0)) num3 = 0;

        if (num1 > 0) return false;
        if (num2 > 0) return false;
        if (num3 > 0) return false;

        int zeros = 0;

        if (is_equal(num1, 0)) ++zeros;
        if (is_equal(num2, 0)) ++zeros;
        if (is_equal(num3, 0)) ++zeros;

        if (zeros > 2) return false;
        return true;
    }
} // <-- double_op