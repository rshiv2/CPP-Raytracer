#include <iostream>
#include <utility>
#include <random>
#include <functional>

#include "../src/rtcore.h"

/* ------------- Test cases ------------- */

bool test_simple_triangle_mesh() {
    
    // construct a tetrahedron with unit vertex locations
    std::vector<point3> vertices;
    vertices.push_back({0,1,0});
    vertices.push_back({1,0,1});
    vertices.push_back({-1,0,1});
    vertices.push_back({0,0,-1});
    

    // construct faces with CCW vertex winding
    face f1, f2, f3, f4;
    f1.vertex_indices = {0,2,1};
    f2.vertex_indices = {0,1,3};
    f3.vertex_indices = {0,3,2};
    f4.vertex_indices = {1,2,3};

    // construct normals
    std::vector<vec3> normals;
    vec3 f1_normal = cross(vertices[2] - vertices[0], vertices[1] - vertices[2]);
    vec3 f2_normal = cross(vertices[1] - vertices[0], vertices[3] - vertices[1]);
    vec3 f3_normal = cross(vertices[3] - vertices[0], vertices[2] - vertices[3]);
    vec3 f4_normal = cross(vertices[2] - vertices[1], vertices[3] - vertices[2]);
    normals.insert(normals.end(), {f1_normal, f2_normal, f3_normal, f4_normal});

    f1.normal_indices = {0,0,0};
    f2.normal_indices = {1,1,1};
    f3.normal_indices = {2,2,2};
    f4.normal_indices = {3,3,3};

    std::vector<face> faces{f1, f2, f3, f4};

    std::shared_ptr<material> mat = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));

    triangle_mesh mesh(vertices, normals, faces, mat);

    // generate ray that will intersect the mesh
    bool passed = true;
    float min_pos = -10.0f, max_pos = 10.0f;
    for (int i = 0; i < 4; i++) {

        face f = faces[i]; 
        vec3 barycentric_weights(random_double(), random_double(), random_double());
        barycentric_weights = barycentric_weights / barycentric_weights.sum();
        point3 point_on_face = vertices[f.vertex_indices[0]] * barycentric_weights[0]
                             + vertices[f.vertex_indices[1]] * barycentric_weights[1]
                             + vertices[f.vertex_indices[2]] * barycentric_weights[2];
        
        point3 normal_at_point = normals[f.normal_indices[0]] * barycentric_weights[0]
                               + normals[f.normal_indices[1]] * barycentric_weights[1]
                               + normals[f.normal_indices[2]] * barycentric_weights[2];

        point3 ray_origin = vec3(point_on_face + normal_at_point);
        vec3 ray_direction = unit_vector(point_on_face - ray_origin);

        ray r(ray_origin, ray_direction);

        /*
        std::cout << "Ray with origin\n" << ray_origin << "\n and direction \n" << ray_direction
                  << " \nexpected to hit triangle with vertices\n " << vertices[std::get<0>(f.vertex_indices)]
                  << ", " << vertices[std::get<1>(f.vertex_indices)] << ", " << vertices[std::get<2>(f.vertex_indices)]
                  << " \n at " << point_on_face << std::endl;
        */

        double t_min = 0, t_max = INF, t_expected = (point_on_face - ray_origin).length();
        hit_record rec;

        passed &= mesh.hit(r, t_min, t_max, rec);

        // if (passed = mesh.hit(r, t_min, t_max, rec)) {
        //     std::cout << "hit a triangle at " << rec.p << std::endl << std::endl;
        // }

        passed &= std::fabs(rec.t - t_expected) < EPS;
    }
    return passed;
}

bool test_triangle_intersection_simple() {

    // very simple collision test
    triangle tri_simple_hit(std::vector<point3>{point3(0, 1, -5), point3(-1, -1, -5), point3(1, -1, -5)});

    ray r_simple_hit(point3(0,0,-1), vec3(0,0,-1));
    double t_min = 0.001, t_max = 100;
    hit_record rec;

    bool expected = true;
    int t_expected = 4;
    bool result = (expected == tri_simple_hit.hit(r_simple_hit, t_min, t_max, rec));
    result &= (rec.t == t_expected);

    return result;
}

bool test_triangle_intersection_random() {
    
    // generate triangle with random vertices
    std::vector<point3> vertices;
    const int min_pos = -10, max_pos = 10;
    for (int i = 0; i < 3; i++) {
        point3 vertex(random_double(min_pos, max_pos), random_double(min_pos, max_pos), random_double(min_pos, max_pos));
        vertices.push_back(vertex);
    }

    triangle tri(vertices);

    // generate random ray
    point3 ray_origin;
    vec3 ray_direction;
    double t_expected;

    point3 random_point_in_triangle;
    bool ray_should_hit_triangle = (random_double() > 0.5);
    if (ray_should_hit_triangle) {
        // generate random ray
        ray_origin = point3(random_double(min_pos, max_pos), random_double(min_pos, max_pos), random_double(min_pos, max_pos));
        vec3 barycentric_weights(random_double(), random_double(), random_double());
        barycentric_weights /= barycentric_weights.sum();
        random_point_in_triangle = barycentric_weights[0] * vertices[0] 
                                        + barycentric_weights[1] * vertices[1] 
                                        + barycentric_weights[2] * vertices[2];
        ray_direction = unit_vector(random_point_in_triangle - ray_origin);
        t_expected = (random_point_in_triangle - ray_origin).length();

    } else {
        ray_origin = point3(random_double(min_pos, max_pos), random_double(min_pos, max_pos), random_double(min_pos, max_pos));

        // get random point on triangle edge
        int vert_index = random_int() % 3;
        point3 v0 = vertices[vert_index];
        point3 v1 = vertices[(vert_index + 1) % 3];
        float t = random_double();
        point3 point_on_tri_edge = v0 + t * (v1 - v0);

        // dither from edge by amount delta
        vec3 centroid_to_edge = point_on_tri_edge - tri.centroid();
        double delta = random_double(1,2);
        point3 point_outside_tri = tri.centroid() + delta * centroid_to_edge;

        ray_direction = unit_vector(point_outside_tri - ray_origin);        
    }
    
    // construct ray and perform intersection test
    ray r(ray_origin, ray_direction);
    hit_record rec;
    double t_min = 0.001, t_max = 100;
    bool result = (ray_should_hit_triangle == tri.hit(r, t_min, t_max, rec));

    // check results
    if (!result) {
        std::cout << "Test failed: " << (ray_should_hit_triangle ? "expected " : "did not expect ")
                  << "ray with:\n" << "origin: " << ray_origin << "\ndirection: " << ray_direction 
                  << "\nto intersect triangle with vertices \n["
                  << vertices[0] << ", " << vertices[1] << ", " << vertices[2] << "]" << std::endl;
    }

    if (ray_should_hit_triangle) {
        float eps = 1e-6;
        result &= std::fabs(rec.t - t_expected) < eps;
        
        if (!result) {
            std::cout << rec.p << ", " << random_point_in_triangle << std::endl;
        }
    }
    return result;
}

bool test_triangle_intersection_watertightness() {

    // generate two triangles with shared edge
    std::vector<point3> vertices;
    const int min_pos = -10, max_pos = 10;
    for (int i = 0; i < 3; i++) {
        point3 vertex(random_double(min_pos, max_pos), random_double(min_pos, max_pos), random_double(min_pos, max_pos));
        vertices.push_back(vertex);
    }

    point3 vert4 = vertices[0] + (vertices[1] - vertices[0]) + (vertices[2] - vertices[0]);
    // vertices.push_back(vert4);
    vec3 tri_normal = unit_vector(cross(vertices[2] - vertices[0], vertices[1] - vertices[0]));
    vertices.push_back(vert4 + (random_double() - 0.5) * 2 * tri_normal);

    triangle tri1({vertices[0], vertices[1], vertices[2]});
    triangle tri2({vertices[2], vertices[1], vertices[3]});

    // generate random ray origin
    point3 ray_origin(random_double(min_pos, max_pos), random_double(min_pos, max_pos), random_double(min_pos, max_pos));
    vec3 ray_direction;

    // get random point on shared triangle edge
    float t = random_double();
    point3 point_on_tri_edge = vertices[1] + t * (vertices[2] - vertices[1]);

    // generate ray
    ray_direction = unit_vector(point_on_tri_edge - ray_origin);
    ray r(ray_origin, ray_direction);

    hit_record rec;
    float t_min = 0.001, t_max = 100, t_expected = (point_on_tri_edge - ray_origin).length();
    bool result = (tri1.hit(r, t_min, t_max, rec) || tri2.hit(r, t_min, t_max, rec));

    return result;

}
bool test_bvh() {
    hittable_list world;

    // create grid of 16 spheres, and 16 rays that will hit the sphere centers
    const int offset = 40;
    const int radius = 10;
    std::vector<std::pair<ray, bool>> test_rays;
    for (int i = -2; i < 2; i++) {
        for (int j = -2; j < 2; j++) {
            point3 center(i * offset, j * offset, offset);
            world.add(std::make_shared<sphere>(center, radius));
            test_rays.push_back(std::make_pair(ray(point3(0,0,0), center), true)); 
        }
    }

    // create rays that will *miss* the sphere centers
    for (int i = -2; i < 2; i++) {
        for (int j = -2; j < 2; j++) {
            point3 center(i*offset + radius*2, j*offset + radius*2, offset);
            test_rays.push_back(std::make_pair(ray(point3(0,0,0), center), false));
        }
    }
    
    // initialize bvh
    world.commit();

    // test ray-bvh intersections
    for (int i = 0; i < test_rays.size(); i++) {
        auto t = test_rays[i];
        hit_record rec;
        ray r = t.first;
        bool expected = t.second;
        bool result = world.hit(r, 0.001, INF, rec);
        if (result != expected) {
            std::cerr << "test_bvh() failed: ray " << r.origin() << ", " << r.direction()
                      << "returned " << std::boolalpha << result << " but expected " << expected 
                      << " (" << i << ")" << std::endl;
            
            return false;
        }
    }

    return true;
}

/* ---------------- Command Line Parsing --------------- */

struct Test {

    Test(const char * name, std::function<bool(void)> func) : test_name(name), test_function(func), passed(false) {};

    void run() {
        if ((this->passed = this->test_function())) {
            std::cout << test_name << " passed" << std::endl;
        } else {
            std::cout << test_name << " failed" << std::endl;
        }
    }

    std::string test_name;
    std::function<bool(void)> test_function;
    bool passed;
};

void print_usage() {
    std::cout << "Usage: \n"
              << "./out [test_name1 test_name2 ...] [-h]\n"
              << std::endl;

    std::cout << "Valid test_name: all, triangle_intersection_simple, triangle_intersection_random\n" 
              << " triangle_intersection_watertightness, simple_triangle_mesh, bvh\n" 
              << std::endl;

    std::cout << "-h --help       This message" << std::endl;
}

std::vector<Test> parse_args(int argc, char* argv[]) {

    std::vector<Test> tests;
    if (argc == 1) {
        tests.push_back(Test("triangle_intersection_simple", test_triangle_intersection_simple));
        tests.push_back(Test("triangle_intersection_random", test_triangle_intersection_random));
        tests.push_back(Test("triangle_intersection_watertightness", test_triangle_intersection_watertightness));
        tests.push_back(Test("simple_triangle_mesh", test_simple_triangle_mesh));
        tests.push_back(Test("bvh", test_bvh));
        return tests;
    }

    for (int i = 1; i < argc; i++) {
        std::string cmd_line_str(argv[i]);

        // determine type of command line option
        if (cmd_line_str == "-h" || cmd_line_str == "--help") {
            print_usage();
            std::vector<Test> empty_vec;
            return empty_vec;
        }

        // determine corresponding command line value
        if (cmd_line_str == "triangle_intersection_simple") {
            tests.push_back(Test("triangle_intersection_simple", test_triangle_intersection_simple));
        } else if (cmd_line_str == "triangle_intersection_random") {
            tests.push_back(Test("triangle_intersection_random", test_triangle_intersection_random));
        } else if (cmd_line_str == "triangle_intersection_watertightness") {
            tests.push_back(Test("triangle_intersection_watertightness", test_triangle_intersection_watertightness));
        } else if (cmd_line_str == "simple_triangle_mesh") {
            tests.push_back(Test("simple_triangle_mesh", test_simple_triangle_mesh));
        } else if (cmd_line_str == "bvh") {
            tests.push_back(Test("bvh", test_bvh));
        } else {
            std::cout << "Unknown argument: " << cmd_line_str << std::endl;
            std::vector<Test> empty_vec;
            return empty_vec;
        }

    }
    return tests;
}

int main(int argc, char* argv[]) {

    std::vector<Test> tests = parse_args(argc, argv);

    int num_tests_passed = 0, num_tests_total = tests.size();
    for (auto& test : tests) {
        test.run();
        num_tests_passed += test.passed;
    }
    
    std::cout << "Tests Passed: " << num_tests_passed << "/" << num_tests_total << std::endl;

    return EXIT_SUCCESS;

    bool passed = test_bvh();

    std::cout << "bvh tests " << (passed ? "passed." : "failed.") << std::endl;

    passed = test_triangle_intersection_simple();

    std::cout << "simple triangle test " << (passed ? "passed." : "failed.") << std::endl; 

    passed = true;
    for (int i = 0; i < 50; i++) {
        passed &= test_triangle_intersection_random();
    }

    std::cout << "random triangle intersection tests " << (passed ? "passed." : "failed.") << std::endl;
    
    passed = true;
    for (int i = 0; i < 50; i++) {
        passed &= test_triangle_intersection_watertightness();
    }

    std::cout << "ray-triangle edge intersection tests " << (passed ? "passed." : "failed.") << std::endl;
    
    passed = test_simple_triangle_mesh();

    std::cout << "simple triangle mesh tests " << (passed ? "passed." : "failed.") << std::endl;

    return EXIT_SUCCESS;
}