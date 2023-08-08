#ifndef HITTABLE_LIST
#define HITTABLE_LIST

#include <vector>
#include <memory>
#include "hittable.h"

#include "bvh.h"

class hittable_list : public hittable {
    public:
        hittable_list() {};
        hittable_list(std::shared_ptr<hittable> object) {
            add(object);
        }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        void add(std::shared_ptr<hittable> object) {
            objects.push_back(object);
        }

        bool commit() {
            return bounding_box(box) && construct_bvh();
        }

        virtual bool bounding_box(aabb& out_box) const override;
        virtual point3 centroid() const override;

    private:
        aabb box;
        std::vector<std::shared_ptr<hittable>> objects;
        std::shared_ptr<bvh_node> node = nullptr;

        bool construct_bvh();
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    if (!box.hit(r, t_min, t_max)) {
        return false;
    }
     
    bool hit_anything = node->hit(r, 0.001, INF, rec);
    return hit_anything;

    /*
    double closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
    */ 
}

bool hittable_list::construct_bvh() {
    // sort list of objects based on centroid
    node = std::make_shared<bvh_node>(objects, 0, objects.size());
    return true;
}

bool hittable_list::bounding_box(aabb& out_box) const {
    if (objects.empty()) {
        return false;
    }

    aabb temp_box;
    bool first_box = true;
    
    for (const auto& object : objects) {
        if (!object->bounding_box(temp_box))
            return false;

        out_box = first_box ? temp_box : surrounding_box(out_box, temp_box);
        first_box = false;
    }

    return true;
}

point3 hittable_list::centroid() const {
    return (box.min() + box.max()) / 2.0f;
}

#endif // HITTABLE_LIST