#ifndef HITTABLE_H
#define HITTABLE_H

#include "vec3.h"
#include "ray.h"
#include "aabb.h"
#include "rtweekend.h"

class material;

struct hit_record {
    point3 p;
    vec3 n;
    double t = INF;
    bool front_face;    // is ray hitting outer side of surface?
    std::shared_ptr<material> mat_ptr;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        n = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
    
    public:
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0; 
        virtual bool bounding_box(aabb& out_box) const = 0;
        virtual point3 centroid() const = 0;

    private:
        aabb box;
};

#endif // HITTABLE_H