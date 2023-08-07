#ifndef BVH_H
#define BVH_H

#include <memory>

#include "hittable.h"

class bvh_node : public hittable {
    public:
        bvh_node() {}
        
        bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects,
                 size_t start, size_t end);

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

        bool is_leaf;

        virtual bool bounding_box(aabb& out_box) const override;
        virtual point3 centroid() const override;

    private:
        std::shared_ptr<hittable> left = nullptr;
        std::shared_ptr<hittable> right = nullptr;
        aabb box;


};

bvh_node::bvh_node(const std::vector<std::shared_ptr<hittable>>& src_objects,
                   size_t start, size_t end) {

    // sort objects along desired axis
    const int axis = 2; // x axis
    auto comparator = [&axis] (std::shared_ptr<hittable>& h1, std::shared_ptr<hittable>& h2) -> bool {
        return h1->centroid()[axis] < h2->centroid()[axis];
    };

    auto objects = src_objects;

    if (!std::is_sorted(objects.begin(), objects.end(), comparator)) {
        std::sort(objects.begin(), objects.end(), comparator);
    }

    // divide objects between left and right children
    if (end - start == 1) {
        left = src_objects[start];
        is_leaf = true;
    } else if (end - start == 2) {
        left = src_objects[start];
        right = src_objects[start+1];
        is_leaf = true;
    } else {
                            
        size_t midpoint = start + (end - start) / 2;
        left = std::make_shared<bvh_node>(src_objects, start, midpoint);
        right = std::make_shared<bvh_node>(src_objects, midpoint, end);
        is_leaf = false;
    }

    // initialize bounding box
    bounding_box(box);
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    if (!box.hit(r, t_min, t_max)) {
        return false;
    } 

    bool hit_anything = false;
    if (left) {
        hit_anything |= left->hit(r, t_min, t_max, rec);
        if (hit_anything) {
            t_max = rec.t;
        }
    }

    if (right) {
        hit_anything |= right->hit(r, t_min, t_max, rec);
    }
    return hit_anything;
}

point3 bvh_node::centroid() const {
    return (box.min() + box.max()) / 2;
}
bool bvh_node::bounding_box(aabb& out_box) const {
    if (is_leaf) {
        if (left && right) {
            aabb left_box, right_box;
            left->bounding_box(left_box);
            right->bounding_box(right_box);
            out_box = surrounding_box(left_box, right_box);
        } else if (left) {
            left->bounding_box(out_box);
        }
        return true;
    }

    aabb left_box, right_box;
    left->bounding_box(left_box);
    right->bounding_box(right_box);
    out_box = surrounding_box(left_box, right_box);
    return true;
}
#endif // BVH_H