#include "moving_sphere.h"

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

bool moving_sphere::create_bounding_box() {
    // assumes that sphere moves in straight line defined by lerp
    // ie. all information on bounds of sphere can be inferred from 
    // endpoints of its movement

    aabb box0(center0 - vec3(radius, radius, radius),
              center0 + vec3(radius, radius, radius));
    
    aabb box1(center1 - vec3(radius, radius, radius),
              center1 + vec3(radius, radius, radius));

    this->box = surrounding_box(box0, box1);
    
    return true;
}

point3 moving_sphere::centroid() const {
    return (center0 + center1) / 2;
}

bool moving_sphere::commit() {
    return create_bounding_box();
}