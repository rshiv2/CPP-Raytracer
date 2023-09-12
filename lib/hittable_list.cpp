#include "hittable_list.h"
#include "bvh.h"

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    if (!box.hit(r, t_min, t_max)) {
        return false;
    }
     
    bool hit_anything = node->hit(r, 0.001, INF, rec);
    return hit_anything;
}

bool hittable_list::construct_bvh() {
    // sort list of objects based on centroid
    node = std::make_shared<bvh_node>(objects, 0, objects.size());
    return true;
}

bool hittable_list::create_bounding_box() {
    if (objects.empty()) {
        return false;
    }

    aabb temp_box, out_box;
    bool first_box = true;
    
    for (auto& object : objects) {
        if (!object->create_bounding_box())
            return false;

        temp_box = object->get_bounding_box();
        out_box = first_box ? temp_box : surrounding_box(out_box, temp_box);
        first_box = false;
    }

    this->box = out_box;
    return true;
}

point3 hittable_list::centroid() const {
    return (box.min() + box.max()) / 2.0f;
}