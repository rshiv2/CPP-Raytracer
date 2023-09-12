#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"
#include "vec3.h"
#include "ray.h"

class aabb {

    public:
        aabb() {}
        aabb(const point3& a, const point3& b) 
            : minimum(a), maximum(b) {
                for (int i = 0; i < 3; i++) {
                    if (minimum[i] == maximum[i]) {
                        minimum[i] -= EPS;
                        maximum[i] += EPS;
                    }
                }
            } 

        point3 min() const { return minimum; }
        point3 max() const { return maximum; }

        bool hit(const ray& r, double t_min, double t_max) const;

    private:
        point3 minimum, maximum;
};

aabb surrounding_box(const aabb& box0, const aabb& box1);
    
#endif // AABB_H

