#ifndef RT_WEEKEND_H
#define RT_WEEKEND_H

#include <limits>
#include <random>

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.14159265358979;
const double EPS = 1e-5; 

static std::mt19937 randomEngine(std::time(0));  // truly random
// static std::mt19937 randomEngine(41);
static std::uniform_int_distribution<int> distribution;
static const double one_over_distribution_max = 1.0 / static_cast<double>(distribution.max());

inline double degrees_to_radians(const double degrees) {
    return degrees * PI / 180.0f;
}

inline int random_int() {
    return distribution(randomEngine);
}
inline double random_double() {
    // return random double in [0,1)
    return distribution(randomEngine) * one_over_distribution_max;
}

inline double random_double(const double min, const double max) {
    // return random double in [min, max)
    return min + random_double() * (max - min);
}

inline int random_sign() {
    // random sample of {-1, 1}
    return random_double() > 0.5 ? 1 : -1;
}

inline double clamp(double x, const double min, const double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

inline bool near_zero(double x) {
    const double eps = 1e-9;
    return std::fabs(x) < eps;
}

#endif // RT_WEEKEND_H