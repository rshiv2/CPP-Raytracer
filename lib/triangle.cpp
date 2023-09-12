#include "triangle.h"
#include "triangle_mesh.h"

triangle::triangle(const triangle_mesh* mesh, 
                   const std::vector<uint32_t>& vertex_indices, 
                   const std::vector<uint32_t>& normal_indices) 
        : parent_mesh(mesh), _vi0(vertex_indices[0]), _vi1(vertex_indices[1]),
          _vi2(vertex_indices[2]), _ni0(normal_indices[0]), _ni1(normal_indices[1]), _ni2(normal_indices[2]) {}

point3 triangle::centroid() const {
    return _centroid;
}

bool triangle::commit() {
    vec3 _v0 = parent_mesh->_vertices[_vi0];
    vec3 _v1 = parent_mesh->_vertices[_vi1];
    vec3 _v2 = parent_mesh->_vertices[_vi2];

    _v0 = transform::apply_transforms(_v0, parent_mesh->_transforms);
    _v1 = transform::apply_transforms(_v1, parent_mesh->_transforms);
    _v2 = transform::apply_transforms(_v2, parent_mesh->_transforms);

    this->_centroid = (_v0 + _v1 + _v2) / 3;
    return create_bounding_box();
}

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    // bounding box test
    if (!box.hit(r, t_min, t_max)) {
        return false;
    }

    vec3 _v0 = parent_mesh->_vertices[_vi0];
    vec3 _v1 = parent_mesh->_vertices[_vi1];
    vec3 _v2 = parent_mesh->_vertices[_vi2];

    _v0 = transform::apply_transforms(_v0, parent_mesh->_transforms);
    _v1 = transform::apply_transforms(_v1, parent_mesh->_transforms);
    _v2 = transform::apply_transforms(_v2, parent_mesh->_transforms);

    // translation vertices based on ray origin
    point3 v0 = _v0, v1 = _v1, v2 = _v2;
    v0 -= r.origin();
    v1 -= r.origin();
    v2 -= r.origin();

    // permute axes so that ray's largest element is the z-element
    int maxDim = 0;
    maxDim = std::fabs(r.direction()[1]) > std::fabs(r.direction()[0]) ? 1 : 0;
    maxDim = std::fabs(r.direction()[2]) > std::fabs(r.direction()[maxDim]) ? 2 : maxDim;
    int kZ = maxDim, kX = (maxDim + 1) % 3, kY = (kX + 1) % 3;

    ray r_permuted(r.origin(), r.direction().permute(kX, kY, kZ));
    v0 = v0.permute(kX, kY, kZ);
    v1 = v1.permute(kX, kY, kZ);
    v2 = v2.permute(kX, kY, kZ);

    // apply a shear that aligns ray direction with x axis
    double sx = -r_permuted.direction()[0] / r_permuted.direction()[2];
    double sy = -r_permuted.direction()[1] / r_permuted.direction()[2];
    double sz = 1.0 / r_permuted.direction()[2];

    v0 += vec3(sx * v0[2], sy * v0[2], 0);
    v0 *= vec3(1, 1, sz);

    v1 += vec3(sx * v1[2], sy * v1[2], 0);
    v1 *= vec3(1, 1, sz);

    v2 += vec3(sx * v2[2], sy * v2[2], 0);
    v2 *= vec3(1, 1, sz);
    
    // determine if intersection occurs by checking if transformed triangle contains (0,0)
    double e0 = v1.x() * v2.y() - v1.y() * v2.x();
    double e1 = v2.x() * v0.y() - v2.y() * v0.x();
    double e2 = v0.x() * v1.y() - v0.y() * v1.x();
    
    if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e1 > 0 || e2 > 0)) {
        return false;
    }

    if (near_zero(e0 + e1 + e2)) {
        return false;
    }

    // get scaled distance to triangle
    double det = e0 + e1 + e2;
    double tScaled = e0 * v0.z() + e1 * v1.z() + e2 * v2.z();
    if (det < 0 && (tScaled >= 0 || tScaled < t_max * det)) {
        return false;
    } else if (det > 0 && (tScaled <= 0 || tScaled > t_max * det)) {
        return false;
    }

    double oneOverDet = 1.0 / det;
    double b0 = e0 * oneOverDet;
    double b1 = e1 * oneOverDet; 
    double b2 = e2 * oneOverDet;

    float t = tScaled * oneOverDet;
    
    // fill in intersection statistics
    vec3 _n0 = parent_mesh->_vertices[_ni0];
    vec3 _n1 = parent_mesh->_vertices[_ni1];
    vec3 _n2 = parent_mesh->_vertices[_ni2];

    _n0 = transform::apply_transforms(_n0, parent_mesh->_transforms);
    _n1 = transform::apply_transforms(_n1, parent_mesh->_transforms);
    _n2 = transform::apply_transforms(_n2, parent_mesh->_transforms);   

    rec.t = t; 
    rec.p = b0 * _v0 + b1 * _v1 + b2 * _v2;
    rec.n = b0 * _n0 + b1 * _n1 + b2 * _n2;  
    rec.mat_ptr = parent_mesh->_mat;
    return true;
}

bool triangle::create_bounding_box() {

    vec3 _v0 = parent_mesh->_vertices[_vi0];
    vec3 _v1 = parent_mesh->_vertices[_vi1];
    vec3 _v2 = parent_mesh->_vertices[_vi2];
    
    _v0 = transform::apply_transforms(_v0, parent_mesh->_transforms);
    _v1 = transform::apply_transforms(_v1, parent_mesh->_transforms);
    _v2 = transform::apply_transforms(_v2, parent_mesh->_transforms);

    double minX = std::fmin(_v0.x(), std::fmin(_v1.x(), _v2.x()));
    double minY = std::fmin(_v0.y(), std::fmin(_v1.y(), _v2.y()));
    double minZ = std::fmin(_v0.z(), std::fmin(_v1.z(), _v2.z()));

    double maxX = std::fmax(_v0.x(), std::fmax(_v1.x(), _v2.x()));
    double maxY = std::fmax(_v0.y(), std::fmax(_v1.y(), _v2.y()));
    double maxZ = std::fmax(_v0.z(), std::fmax(_v1.z(), _v2.z()));

    this->box = aabb(point3(minX, minY, minZ), point3(maxX, maxY, maxZ));
    return true;
}