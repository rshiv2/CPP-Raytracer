#ifndef LOADER_H
#define LOADER_H

#include "triangle_mesh.h"
#include "vec3.h"
#include "material.h"

std::shared_ptr<triangle_mesh> get_triangle_mesh_from_file(std::string filename, std::shared_ptr<material> mat = nullptr); 

#endif // LOADER_H