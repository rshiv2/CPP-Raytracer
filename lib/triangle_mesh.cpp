#include "triangle_mesh.h"

triangle_mesh::triangle_mesh(const std::vector<point3>& vertices, 
    const std::vector<vec3>& normals, 
    const std::vector<face>& faces,
    std::shared_ptr<material> m)
    : _vertices(vertices), _normals(normals), _mat(m) {
    
    for (auto& face : faces) {
        // each face contains indices to vertex and normal vectors
        _triangles.push_back(std::make_shared<triangle>(this, face.vertex_indices, face.normal_indices));
    }
}

triangle_mesh::triangle_mesh(const std::vector<point3>& vertices, 
    const std::vector<vec3>& normals, 
    const std::vector<vec3>& texture_coords,
    const std::vector<face>& faces,
    std::shared_ptr<material> m)
    : _vertices(vertices), _normals(normals), _texture_coords(texture_coords), _mat(m) {

    for (auto& face : faces) {
        _triangles.push_back(std::make_shared<triangle>(this, face.vertex_indices, face.normal_indices));
    } 
}

triangle_mesh::triangle_mesh(const std::vector<point3>& vertices, 
    const std::vector<vec3>& normals, 
    const std::vector<face>& faces, 
    std::shared_ptr<material> m,
    const std::vector<std::shared_ptr<transform>> transforms)
    : _vertices(vertices), _normals(normals), _mat(m), _transforms(transforms) {

    for (auto& face : faces) {
        _triangles.push_back(std::make_shared<triangle>(this, face.vertex_indices, face.normal_indices));
    } 
} 

bool triangle_mesh::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    if (!box.hit(r, t_min, t_max)) {
        return false;
    }

    // use BVH to determine ray-triangle intersection
    return node->hit(r, t_min, t_max, rec);
}

point3 triangle_mesh::centroid() const {
    return (box.min() + box.max()) / 2;
}

bool triangle_mesh::create_bounding_box() {
    double maxX = -INF, maxY = -INF, maxZ = -INF;
    double minX = INF, minY = INF, minZ = INF; 
    
    for (auto v : _vertices) {
        v = transform::apply_transforms(v, _transforms);

        maxX = std::fmax(v[0], maxX);
        maxY = std::fmax(v[1], maxY);
        maxZ = std::fmax(v[2], maxZ);

        minX = std::fmin(v[0], minX);
        minY = std::fmin(v[1], minY);
        minZ = std::fmin(v[2], minZ);
    }

    this->box = aabb(point3(minX, minY, minZ), point3(maxX, maxY, maxZ));
    return true;
}

bool triangle_mesh::commit() {
    for (auto& tri : _triangles) {
        if (!tri->commit())
            return false;
    }
    this->node = std::make_shared<bvh_node>(_triangles, 0, _triangles.size());
    return create_bounding_box() && (this->node != nullptr);
}