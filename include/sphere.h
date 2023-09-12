#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include "transform.h"

class sphere : public hittable {
    public:
        sphere() {}
        sphere(const point3& c, const double r) :
            center(c), radius(r), mat_ptr(nullptr)  {
                create_bounding_box();
            };
        sphere(const point3&c, const double r, std::shared_ptr<material> m)
            : center(c), radius(r), mat_ptr(m) {
                create_bounding_box();
            };

        sphere(const point3& c, const double r, std::shared_ptr<material> m, const std::vector<std::shared_ptr<transform>>& transforms)
            : center(c), radius(r), mat_ptr(m), _transforms(transforms) {
                create_bounding_box();
            }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool create_bounding_box() override; 
        virtual point3 centroid() const override;
        virtual bool commit() override;
        virtual aabb get_bounding_box() const override {
            return this->box;
        }

        void set_mat_ptr(std::shared_ptr<material> m) { mat_ptr = m; }    


    private:
        aabb box;
        point3 center;
        double radius;
        std::shared_ptr<material> mat_ptr;
        std::vector<std::shared_ptr<transform>> _transforms;

};

#endif // SPHERE_H