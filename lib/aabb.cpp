#include "aabb.h"

bool aabb::hit(const ray& r, double t_min, double t_max) const {
    for (int i = 0; i < 3; i++) {
        auto invD = 1.0f / r.direction()[i];
        
        auto t0 = std::fmin((min()[i] - r.origin()[i]) * invD, (max()[i] - r.origin()[i]) * invD);
        auto t1 = std::max((min()[i] - r.origin()[i]) * invD, (max()[i] - r.origin()[i]) * invD);

        t_min = std::max(t0, t_min);
        t_max = std::min(t1, t_max);

        if (t_min >= t_max) return false;
    }

    return true;
}

aabb surrounding_box(const aabb& box0, const aabb& box1) {
    point3 min(std::fmin(box0.min().x(), box1.min().x()),
               std::fmin(box0.min().y(), box1.min().y()),
               std::fmin(box0.min().z(), box1.min().z()));

    point3 max(std::fmax(box0.max().x(), box1.max().x()),
               std::fmax(box0.max().y(), box1.max().y()),
               std::fmax(box0.max().z(), box1.max().z()));

   aabb out_box(min, max);
   return out_box; 
}