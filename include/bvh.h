#ifndef BVH_H
#define BVH_H

#include <memory>

#include "hittable.h"
#include "aabb.h"
#include "vec3.h"
#include "ray.h"

class bvh_node : public hittable {
    public:
        bvh_node() {}
        
        bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects,
                 size_t start, size_t end);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool create_bounding_box() override;
        virtual point3 centroid() const override;
        virtual bool commit() override { return true; }
        virtual aabb get_bounding_box() const override {
            return box;
        }

        bool is_leaf;

    private:
        std::shared_ptr<hittable> left = nullptr;
        std::shared_ptr<hittable> right = nullptr;
        aabb box;
};


#endif // BVH_H