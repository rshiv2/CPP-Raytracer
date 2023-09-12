#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H

#include "triangle.h"
#include "bvh.h"
#include "transform.h"
#include <cassert>

struct face {
    std::vector<uint32_t> vertex_indices;
    std::vector<uint32_t> texture_indices;
    std::vector<uint32_t> normal_indices;
};

class triangle_mesh : public hittable {

    public:

        triangle_mesh(const std::vector<point3>& vertices, 
                      const std::vector<vec3>& normals, 
                      const std::vector<face>& faces, 
                      std::shared_ptr<material> m);

        triangle_mesh(const std::vector<point3>& vertices, 
                      const std::vector<vec3>& normals, 
                      const std::vector<face>& faces, 
                      std::shared_ptr<material> m,
                      const std::vector<std::shared_ptr<transform>> transforms);

        triangle_mesh(const std::vector<point3>& vertices, 
                      const std::vector<vec3>& normals, 
                      const std::vector<vec3>& texture_coords, 
                      const std::vector<face>& faces, 
                      std::shared_ptr<material> m);
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual point3 centroid() const override;
        virtual bool create_bounding_box() override;
        virtual bool commit() override;
        virtual aabb get_bounding_box() const override {
            return box;
        }

        void set_material(const std::shared_ptr<material> other) {
            _mat = other;
        }
        void to_origin() {
            create_bounding_box();  // must do this before computing centroid
            point3 _centroid = centroid();
            _transforms.push_back(std::make_shared<translation>(-_centroid));
        }

        void push_transform(const std::shared_ptr<transform> t) {
            _transforms.push_back(t);
        }

        void pop_transform() {
            if (_transforms.size() > 0) {
                _transforms.pop_back();
            }
        }

    private:
        aabb box;
        std::shared_ptr<bvh_node> node = nullptr;
        std::vector<point3> _vertices;
        std::vector<vec3> _normals;
        std::vector<vec3> _texture_coords;  // not implemented yet
        std::vector<std::shared_ptr<hittable>> _triangles;
        std::shared_ptr<material> _mat;
        std::vector<std::shared_ptr<transform>> _transforms;

    friend class triangle;

};


#endif // TRIANGLE_MESH_H
