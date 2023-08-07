#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(const point3& c, const double r) :
            center(c), radius(r), mat_ptr(nullptr)  {
                bounding_box(box);
            };
        sphere(const point3&c, const double r, std::shared_ptr<material> m)
            : center(c), radius(r), mat_ptr(m) {
                bounding_box(box);
            };

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        void set_mat_ptr(std::shared_ptr<material> m) { mat_ptr = m; }    

        virtual bool bounding_box(aabb& out_box) const override; 
        virtual point3 centroid() const override;

    private:
        point3 center;
        double radius;
        std::shared_ptr<material> mat_ptr;
        aabb box;

};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max)) {
        return false;
    } 
    const vec3 oc = r.origin() - center;
    const float a = r.direction().length_squared();
    const float half_b = dot(r.direction(), oc);
    const float c = oc.length_squared() - radius * radius;
    const float discriminant = half_b * half_b - a * c;
    
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            return false;
        }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - this->center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    
    return true;
}

bool sphere::bounding_box(aabb& out_box) const {
    vec3 radius_vec(radius, radius, radius);
    point3 min(center - radius_vec);
    point3 max(center + radius_vec);
    out_box = aabb(min, max);
    return true;
}

point3 sphere::centroid() const {
    return center;
}

#endif // SPHERE_H