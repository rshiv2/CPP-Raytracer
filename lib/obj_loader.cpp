#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <exception>

#include "obj_loader.h"

typedef enum OBJElement {
    VERTEX,
    NORMAL,
    TEXTURE,
    FACE
} OBJElement;

static std::map<std::string, OBJElement> string_to_obj_element {
    { "v", VERTEX },
    { "vn", NORMAL },
    { "vt", TEXTURE },
    { "f", FACE }
};

inline point3 read_vertex(std::vector<std::string>& tokens) {
    point3 v(std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3]));
    return v;
}

inline vec3 read_normal(std::vector<std::string>& tokens) {
    vec3 n(std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3]));
    return n;
}

inline vec3 read_texture_coords(std::vector<std::string>& tokens) {
    vec3 t;
    if (tokens.size() == 3) {
        t = vec3{std::stod(tokens[1]), std::stod(tokens[2]), 0};
    } else if (tokens.size() == 4) {
        t = vec3{std::stod(tokens[1]), std::stod(tokens[2]), std::stod(tokens[3])};
    } else {
        throw std::runtime_error("Received texture coordinates that are neither 2D nor 3D");
    }
    return t;
}

inline face read_face(std::vector<std::string>& tokens) {
    
    // face tokens are further delimited by '/'
    // f 1/2/3 means "face with vertex index 1, texture index 2, normal index 3"
    std::vector<uint32_t> vertex_indices;
    std::vector<uint32_t> normal_indices;
    std::vector<uint32_t> texture_indices;

    for (int i = 1; i < tokens.size(); i++) {
        std::string token = tokens[i];
        std::istringstream iss(token);

        // parse token for '/'-delimited sub-tokens
        std::vector<std::string> indices;
        for (std::string index; std::getline(iss, index, '/'); ) {
            indices.push_back(index);
        }

        vertex_indices.push_back(std::stoi(indices[0]) - 1);
        if (indices.size() >= 2 && !indices[1].empty()) texture_indices.push_back(std::stoi(indices[1]) - 1);
        if (indices.size() == 3) normal_indices.push_back(std::stoi(indices[2]) - 1);
            
    }

    face f;
    f.vertex_indices = vertex_indices;
    f.normal_indices = normal_indices;
    return f;
}

std::shared_ptr<triangle_mesh> get_triangle_mesh_from_file(std::string filename, std::shared_ptr<material> mat) {

    std::ifstream infile(filename);

    std::vector<point3> vertices;
    std::vector<vec3> normals;
    std::vector<vec3> texture_coords;
    std::vector<face> faces;

    for (std::string line; std::getline(infile, line); ) {

        // retrieve line from file
        std::istringstream iss(line);
        std::vector<std::string> tokens;

        // split into tokens
        for (std::string token; std::getline(iss, token, ' '); ) {
            if (token.empty()) continue;
            if (token[0] == '#') break;
            
            tokens.push_back(token);
        }

        // read mesh data
        for (int i = 0; i < tokens.size(); i++) {
            std::string token = tokens[i];

            if (i==0) {
                if (string_to_obj_element.find(token) == string_to_obj_element.end()) break;
                switch(string_to_obj_element[token]) {
                    case VERTEX:
                        vertices.push_back(read_vertex(tokens));
                        break;
                    case TEXTURE:
                        texture_coords.push_back(read_texture_coords(tokens));
                        break;
                    case NORMAL:
                        normals.push_back(read_normal(tokens));
                        break;
                    case FACE:
                        faces.push_back(read_face(tokens));
                        break;
                }
            }
        }
    }

    auto mesh = std::make_shared<triangle_mesh>(vertices, normals, texture_coords, faces, mat);
    return mesh;
}