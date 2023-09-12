#include "sphere.h"

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max)) {
        return false;
    } 

    const vec3 center_transformed = transform::apply_transforms(center, _transforms);

    const vec3 oc = r.origin() - center_transformed;
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

bool sphere::create_bounding_box() {
    const vec3 center_transformed = transform::apply_transforms(center, _transforms);
    vec3 radius_vec(radius, radius, radius);
    point3 min(center_transformed - radius_vec);
    point3 max(center_transformed + radius_vec);
    this->box = aabb(min, max);
    return true;
}

point3 sphere::centroid() const {
    return transform::apply_transforms(center, _transforms);
}

bool sphere::commit() {
    return create_bounding_box();
}
