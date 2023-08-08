#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H
#include "hittable_list.h"
#include "triangle.h"


struct face {
    std::vector<int> vertex_indices;
    std::vector<int> texture_indices;
    std::vector<int> normal_indices;
};

class triangle_mesh : public hittable {

    public:

        triangle_mesh(const std::vector<point3>& vertices, const std::vector<vec3>& normals, const std::vector<face>& faces, std::shared_ptr<material> m);
        triangle_mesh(const std::vector<point3>& vertices, const std::vector<vec3>& normals, const std::vector<vec3>& texture_coords, const std::vector<face>& faces, std::shared_ptr<material> m);
        
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        virtual point3 centroid() const override;
        virtual bool bounding_box(aabb& out_box) const override;

    private:
        aabb box;

        std::vector<point3> _vertices;
        std::vector<vec3> _normals;
        std::vector<vec3> _texture_coords;  // not implemented yet
        std::vector<face> _faces;
        std::shared_ptr<material> _mat;
};

triangle_mesh::triangle_mesh(const std::vector<point3>& vertices, 
    const std::vector<vec3>& normals, 
    const std::vector<face>& faces,
    std::shared_ptr<material> m)
    : _vertices(vertices), _normals(normals), _faces(faces), _mat(m) {
    
    bounding_box(box);
}

triangle_mesh::triangle_mesh(const std::vector<point3>& vertices, 
    const std::vector<vec3>& normals, 
    const std::vector<vec3>& texture_coords,
    const std::vector<face>& faces,
    std::shared_ptr<material> m)
    : _vertices(vertices), _normals(normals), _texture_coords(texture_coords), _faces(faces), _mat(m) {
    
    bounding_box(box);
}

bool triangle_mesh::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max)) {
        return false;
    }

    bool hit_anything = false;

    for (int i = 0; i < _faces.size(); i++) {

        face f = _faces[i];
        // get face vertices
        point3 v0 = _vertices[f.vertex_indices[0]];
        point3 v1 = _vertices[f.vertex_indices[1]];
        point3 v2 = _vertices[f.vertex_indices[2]];

        // get vertex normals
        vec3 n0 = _normals[f.normal_indices[0]];
        vec3 n1 = _normals[f.normal_indices[2]];
        vec3 n2 = _normals[f.normal_indices[3]];

        triangle tri(std::vector<point3>{v0, v1, v2}, std::vector<vec3>{n0, n1, n2}, _mat);
        bool tri_was_hit = tri.hit(r, t_min, t_max, rec);
        if (tri_was_hit) {
            hit_anything = true;
            t_max = std::fmin(t_max, rec.t);
        }
    }

    return hit_anything;
}

point3 triangle_mesh::centroid() const {
    return (box.min() + box.max()) / 2;
}

bool triangle_mesh::bounding_box(aabb& out_box) const {
    double maxX = -INF, maxY = -INF, maxZ = -INF;
    double minX = INF, minY = INF, minZ = INF; 
    
    for (auto& v : _vertices) {

        maxX = std::fmax(v[0], maxX);
        maxY = std::fmax(v[1], maxY);
        maxZ = std::fmax(v[2], maxZ);

        minX = std::fmin(v[0], minX);
        minY = std::fmin(v[1], minY);
        minZ = std::fmin(v[2], minZ);
    }

    out_box = aabb(point3(minX, minY, minZ), point3(maxX, maxY, maxZ));
    return true;
}
#endif // TRIANGLE_MESH_H
