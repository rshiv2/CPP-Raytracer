#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "aabb.h"
#include "material.h"
#include <vector>
#include <tuple>

//#define DEBUG
typedef enum winding {
    NONE = 0,
    CLOCKWISE,
    COUNTER_CLOCKWISE
} winding;

class triangle : public hittable {

    public:

        triangle(const std::vector<point3>& vertices) : triangle(vertices, nullptr) {};
        triangle(const std::vector<point3>& vertices, std::shared_ptr<material> m);

        /* delete later. doesn't make sense to construct a triangle without a material property */
        triangle(const std::vector<point3>& vertices, const std::vector<vec3>& normals) : triangle(vertices, normals, nullptr) {};

        triangle(const std::vector<point3>& vertices, const std::vector<vec3>& normals, const std::shared_ptr<material> m); 
        bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
        bool bounding_box(aabb& out_box) const override;
        point3 centroid() const override { return _centroid; };

    private:
        
        aabb box;
        std::shared_ptr<material> _mat;
        point3 _centroid;
        point3 _v0, _v1, _v2;
        vec3 _n0, _n1, _n2;     // per-vertex normals
        vec3 _normal;           // triangle normal

        winding _winding = NONE;   // enforces ordering on _v0, _v1, _v2

};

triangle::triangle(const std::vector<point3>& vertices, const std::vector<vec3>& normals, 
    const std::shared_ptr<material> m) : _v0(vertices[0]), _v1(vertices[1]), _v2(vertices[2]), 
    _n0(normals[0]), _n1(normals[1]), _n2(normals[2]), _mat(m) {

    _centroid = (_v0 + _v1 + _v2) / 3;
    _normal = unit_vector(cross(_v1 - _v0, _v2 - _v1)) * (_winding == COUNTER_CLOCKWISE ? 1 : -1);
    bounding_box(box);
}


triangle::triangle(const std::vector<point3>& vertices, std::shared_ptr<material> m) 
    : _v0(vertices[0]), _v1(vertices[1]), _v2(vertices[2]) {

    // assign face normal to each vertex
    vec3 _normal = unit_vector(cross(_v1 - _v0, _v2 - _v1)) * (_winding == CLOCKWISE ? -1 : 1);
    _n0 = _normal;
    _n1 = _normal;
    _n2 = _normal;

    _mat = m;

    _centroid = (_v0 + _v1 + _v2) / 3;
    
    bounding_box(box);
}

bool triangle::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    
    // backface culling
    if (_winding != NONE && dot(r.direction(), _normal) > 0) {
        return false;
    }
    // translation vertices based on ray origin
    point3 v0 = _v0, v1 = _v1, v2 = _v2;
    v0 -= r.origin();
    v1 -= r.origin();
    v2 -= r.origin();

    #ifdef DEBUG
        std::cout << "Translated vertices: \n\t" << v0 << "\n\t" << v1 << "\n\t" << v2 << "\n" << std::endl;
    #endif

    // permute axes so that ray's largest element is the z-element
    int maxDim = 0;
    maxDim = std::fabs(r.direction()[1]) > std::fabs(r.direction()[0]) ? 1 : 0;
    maxDim = std::fabs(r.direction()[2]) > std::fabs(r.direction()[maxDim]) ? 2 : maxDim;
    int kZ = maxDim, kX = (maxDim + 1) % 3, kY = (kX + 1) % 3;

    ray r_permuted(r.origin(), r.direction().permute(kX, kY, kZ));
    v0 = v0.permute(kX, kY, kZ);
    v1 = v1.permute(kX, kY, kZ);
    v2 = v2.permute(kX, kY, kZ);

    #ifdef DEBUG
        std::cout << "MaxDim: " << maxDim << "\n" << "Permuted vertices: \n\t" << v0 << "\n\t" << v1 << "\n\t" << v2 << "\n" << std::endl;
    #endif

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
    
    #ifdef DEBUG
        std::cout << "Sheared vertices: \n\t" << v0 << "\n\t" << v1 << "\n\t" << v2 << "\n" << std::endl;
    #endif

    // determine if intersection occurs by checking if transformed triangle contains (0,0)
    double e0 = v1.x() * v2.y() - v1.y() * v2.x();
    double e1 = v2.x() * v0.y() - v2.y() * v0.x();
    double e2 = v0.x() * v1.y() - v0.y() * v1.x();
    
 
    //std::cout << "edge weights: " << e0 << ", " << e1 << ", " << e2 << std::endl;

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

    double b0 = e0 / det;
    double b1 = e1 / det; 
    double b2 = e2 / det;

    float t = tScaled / det;
    
    // fill in intersection statistics
    rec.t = t; 
    rec.p = b0 * _v0 + b1 * _v1 + b2 * _v2;
    rec.n = b0 * _n0 + b1 * _n1 + b2 * _n2;  
    rec.mat_ptr = _mat;
    return true;

}

bool triangle::bounding_box(aabb& box) const {

    double minX = std::fmin(_v0.x(), std::fmin(_v1.x(), _v2.x()));
    double minY = std::fmin(_v0.y(), std::fmin(_v1.y(), _v2.y()));
    double minZ = std::fmin(_v0.z(), std::fmin(_v1.z(), _v2.z()));

    double maxX = std::fmax(_v0.x(), std::fmax(_v1.x(), _v2.x()));
    double maxY = std::fmax(_v0.y(), std::fmax(_v1.y(), _v2.y()));
    double maxZ = std::fmax(_v0.z(), std::fmax(_v1.z(), _v2.z()));

    box = aabb(point3(minX, minY, minZ), point3(maxX, maxY, maxZ));
    return true;
}

#endif // TRIANGLE_H