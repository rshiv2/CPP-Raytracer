#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "aabb.h"
#include "material.h"
#include <vector>
#include <tuple>

typedef enum winding {
    NONE = 0,
    CLOCKWISE,
    COUNTER_CLOCKWISE
} winding;

class triangle_mesh;    // forward declaration

class triangle : public hittable {

    public:

        triangle(const triangle_mesh* mesh, const std::vector<uint32_t>& vertex_indices, const std::vector<uint32_t>& normal_indices); 

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual bool create_bounding_box() override;
        virtual point3 centroid() const override;
        virtual bool commit() override;
        virtual aabb get_bounding_box() const override {
            return box;
        }

    private:
        
        aabb box;
        const triangle_mesh* parent_mesh;

        point3 _centroid;
        uint32_t _vi0, _vi1, _vi2;      // vertex indices (index into parent_mesh's vertex list)
        uint32_t _ni0, _ni1, _ni2;      // normal indices (index into parent_mesh's normal list)

        winding _winding = NONE;   // enforces ordering on _v0, _v1, _v2

};


#endif // TRIANGLE_H