#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "rtweekend.h"

class vec3 {
    public:

        vec3() : e{0,0,0} {};
        vec3(double x, double y, double z) : e{x, y, z} {};

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        vec3 operator- () const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[] (int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        vec3& operator+= (const vec3 &v) {
            e[0] += v[0];
            e[1] += v[1];
            e[2] += v[2];
            return *this;
        }

        vec3& operator+= (const double t) {
            e[0] += t;
            e[1] += t;
            e[2] += t;
            return *this;
        }

        vec3& operator -= (const vec3 &v) {
            e[0] -= v[0];
            e[1] -= v[1];
            e[2] -= v[2];
            return *this;
        }

        vec3& operator -= (const double t) {
            e[0] -= t;
            e[1] -= t;
            e[2] -= t;
            return *this;
        }

        vec3& operator*= (const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3& operator*= (const vec3 &v) {
            e[0] *= v[0];
            e[1] *= v[1];
            e[2] *= v[2];
            return *this;
        }

        vec3& operator/= (const double t) {
            return *this *= 1/t;
        }

        double length() const {
            return std::sqrt(length_squared());
        }

        double length_squared() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        bool near_zero() const {
            const auto s = 1e-8;
            return (fabs(e[0]) < s) && (fabs(e[1] < s)) && (fabs(e[2] < s));
        }

        vec3 permute(int x, int y, int z) {
            if (x < 0 || x > 2 || y < 0 || y > 2 || z < 0 || z > 2) {
                throw "vec3::permute() received invalid arguments";
            }

            if (x == y || x == z || y == z) {
                throw "veec3::permute() received invalid arguments";
            }

            vec3 v(e[x], e[y], e[z]);
            return v;
        }

        double sum() const {
            return e[0] + e[1] + e[2];
        }

        double avg() const {
            return sum() / 3;
        }

        inline static vec3 random() {
            return vec3(random_double(), random_double(), random_double());
        }

        inline static vec3 random(double min, double max) {
            return vec3(random_double(min, max), random_double(min, max), random_double(min,max));
        }

        static vec3 random_in_unit_sphere() {
            while (true) {
                vec3 v = vec3::random(-1.0, 1.0);
                if (v.length_squared() < 1.0) return v;
            }
        }

        static vec3 random_in_unit_disk() {
            while (true) {
                auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
                if (p.length_squared() < 1) return p;
            }
        }

    private:

        double e[3];

    friend std::ostream& operator<<(std::ostream& os, const vec3& v);
};

inline std::ostream& operator<<(std::ostream& os, const vec3& v) {
    return os << "(" << v.e[0] << ", " << v.e[1] << ", " << v.e[2] << ")";
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u[0] + v[0], u[1] + v[1], u[2] + v[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v)
{
    return vec3(u[0] - v[0], u[1] - v[1], u[2] - v[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline vec3 operator*(const vec3& v, const double t) {
    return vec3(t * v[0], t * v[1], t * v[2]);
}

inline vec3 operator*(const double t, const vec3& v) {
    return vec3(t * v[0], t * v[1], t * v[2]);
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u[1] * v[2] - u[2] * v[1],
                u[2] * v[0] - u[0] * v[2],
                u[0] * v[1] - u[1] * v[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

using point3 = vec3;
using color = vec3;

#endif // VEC3_H
