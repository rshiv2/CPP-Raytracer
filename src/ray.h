#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class ray{
    public:
        ray() {}
        ray(const point3& origin, const vec3& direction, const double time = 0.0) 
            : orig(origin), dir(unit_vector(direction)), tm(time) {}
        
        point3 origin() const { return orig; }
        vec3 direction() const { return dir; }

        point3 at(double t) const {
            return orig + t * dir;
        }

        point3 operator() (double t) const {
            return orig + t * dir;
        }

        inline double time() const {
            return tm;
        }

    private:
        point3 orig; // origin
        vec3 dir;    // direction
        double tm;   // time
};

#endif // RAY_H