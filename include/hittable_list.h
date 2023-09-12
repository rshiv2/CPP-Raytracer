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

        virtual bool commit() override {
            for (auto& object : objects) {
                if (!object->commit()) {
                    return false;
                }
            }
            return create_bounding_box() && construct_bvh();
        }
        virtual bool create_bounding_box() override;
        virtual point3 centroid() const override;
        virtual aabb get_bounding_box() const override {
            return box;
        }

    private:
        aabb box;
        std::vector<std::shared_ptr<hittable>> objects;
        std::shared_ptr<bvh_node> node = nullptr;

        bool construct_bvh();
};

#endif // HITTABLE_LIST