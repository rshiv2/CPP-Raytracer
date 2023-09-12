#ifndef MOVING_SPHERE_H 
#define MOVING_SPHERE_H

#include "hittable.h"
#include "vec3.h"

class moving_sphere : public hittable {
    public:
        moving_sphere() {}

        moving_sphere(const point3& c0, const point3& c1, const double r, std::shared_ptr<material> m, const double t0, const double t1)
            : center0(c0), center1(c1), radius(r), mat_ptr(m), time0(t0), time1(t1) {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool create_bounding_box() override;
        virtual point3 centroid() const override;
        virtual bool commit() override;
        virtual aabb get_bounding_box() const override {
            return box;
        }

        void set_mat_ptr(std::shared_ptr<material> m) { mat_ptr = m; }    

        inline point3 center(double t) const{
            return center0 + ((t - time0) / (time1 - time0)) * (center1 - center0);
        }

    private:
        point3 center0, center1;
        double time0, time1;
        double radius; std::shared_ptr<material> mat_ptr; 
        aabb box;

};

#endif // MOVING_SPHERE_H

