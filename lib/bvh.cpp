#include "bvh.h"

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
    create_bounding_box();
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

bool bvh_node::create_bounding_box() {
    if (is_leaf) {
        if (left && right) {
            aabb left_box, right_box;
            left_box = left->get_bounding_box();
            right_box = right->get_bounding_box();
            this->box = surrounding_box(left_box, right_box);
        } else if (left) {
            this->box = left->get_bounding_box();
        }
        return true;
    }

    aabb left_box = left->get_bounding_box();
    aabb right_box = right->get_bounding_box();
    this->box = surrounding_box(left_box, right_box);
    return true;
}