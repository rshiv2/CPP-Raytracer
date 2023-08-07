#ifndef MOVING_SPHERE_H 
#define MOVING_SPHERE_H

#include "hittable.h"
#include "vec3.h"

class moving_sphere : public hittable {
    public:
        moving_sphere() {}

        moving_sphere(const point3& c0, const point3& c1, const double r, std::shared_ptr<material> m, const double t0, const double t1)
            : center0(c0), center1(c1), radius(r), mat_ptr(m), time0(t0), time1(t1) {
                bounding_box(box);
            };

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        void set_mat_ptr(std::shared_ptr<material> m) { mat_ptr = m; }    

        virtual bool bounding_box(aabb& out_box) const override;
        virtual point3 centroid() const override;

        inline point3 center(double t) const{
            return center0 + ((t - time0) / (time1 - time0)) * (center1 - center0);
        }

    private:
        point3 center0, center1;
        double time0, time1;
        double radius; std::shared_ptr<material> mat_ptr; 
        aabb box;

};

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    if (!box.hit(r, t_min, t_max)) {
        return false;
    }
    
    const vec3 oc = r.origin() - center(r.time());
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
    vec3 outward_normal = (rec.p - center(r.time())) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;
    
    return true;
};

bool moving_sphere::bounding_box(aabb& out_box) const {
    // assumes that sphere moves in straight line defined by lerp
    // ie. all information on bounds of sphere can be inferred from 
    // endpoints of its movement

    aabb box0(center0 - vec3(radius, radius, radius),
              center0 + vec3(radius, radius, radius));
    
    aabb box1(center1 - vec3(radius, radius, radius),
              center1 + vec3(radius, radius, radius));

    out_box = surrounding_box(box0, box1);
    
    return true;
}

point3 moving_sphere::centroid() const {
    return (center0 + center1) / 2;
}

#endif // MOVING_SPHERE_H

