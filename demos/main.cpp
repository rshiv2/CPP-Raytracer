#include <iostream>
#include <thread>
#include <utility>

#include "rtcore.h"
#include "renderer.h"

color ray_color(const ray& r, const hittable_list& world, int depth) 
{
    if (depth <= 0) {
        return color(0,0,0);
    }

    hit_record rec;
    if (world.hit(r, 0.001, INF, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        return color(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {

            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;
                auto radius = 0.2;
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = std::make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,0.5), 0);
                    world.add(std::make_shared<sphere>(center, radius, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, radius, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, radius, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    world.commit();

    return world;
}

hittable_list sphere_scene() {
    hittable_list world;

    auto material1 = std::make_shared<dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    std::vector<std::shared_ptr<transform>> transforms2 = { std::make_shared<translation>(-4,0,0) };
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material2, transforms2));

    auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    std::vector<std::shared_ptr<transform>> transforms3 = { std::make_shared<translation>(4,0,0) };
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material3, transforms3));

    try {
        if (!world.commit()) {
            throw "Failed to commit hittable list";
        }
    } catch (std::string e) {
        std::cout << e << std::endl;
    }

    return world;
}

hittable_list tetra_scene() {
    hittable_list world;

    color marble(0.949f, 0.941f, 0.902f);
    color blue_marble(201, 217, 232);
    blue_marble /= 255.0f;
    auto mesh_material = std::make_shared<glossy>(blue_marble, blue_marble, 0.0, 1.0);
    auto mesh = get_triangle_mesh_from_file("../../objs/tetrahedron.obj", mesh_material);
    world.add(mesh);

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.25, 0.25));
    world.add(std::make_shared<sphere>(point3(0,-1000.5,0), 1000, ground_material));

    auto material1 = std::make_shared<glossy>(marble, marble, 0.5, 0.8);
    auto center1 = mesh->centroid() + point3(-4,0,0);
    world.add(std::make_shared<sphere>(center1, 1.0, material1));

    auto material2 = std::make_shared<glossy>(marble, marble, 0.8, 0.8);
    auto center2 = mesh->centroid() + point3(4,0,0);
    world.add(std::make_shared<sphere>(center2, 1.0, material2));
    world.commit();
    return world;
}

hittable_list teapot_scene() {
    hittable_list world;

    color marble(0.949f, 0.941f, 0.902f);
    auto lambertian_material = std::make_shared<lambertian>(color(0.0,1.0,0.0));

    for (int i = 0; i <= 4; i++) {
        auto mesh = get_triangle_mesh_from_file("../../objs/teapot.obj");
        auto material = std::make_shared<glossy>(marble, marble, 0.8, i / 4.0f);
        mesh->set_material(material);
        mesh->push_transform(std::make_shared<scale>(0.5f, 0.5f, 0.5f));
        mesh->push_transform(std::make_shared<translation>(3.0f * (i-2), 0, 0));
        world.add(mesh);
    }

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.25, 0.25));
    world.add(std::make_shared<sphere>(point3(0,-1000.5,0), 1000, ground_material));
    
    /*
    auto material1 = std::make_shared<glossy>(marble, marble, 0.5, 0.8);
    auto center1 = mesh_centroid + point3(-4,0,0);
    world.add(std::make_shared<sphere>(center1, 1.0, material1));

    auto material2 = std::make_shared<glossy>(marble, marble, 0.8, 0.8);
    auto center2 = mesh_centroid + point3(4,0,0);
    world.add(std::make_shared<sphere>(center2, 1.0, material2));
    */

    world.commit();
    return world;
}

void compute_pixel_color(const std::vector<std::pair<int,int>>& pixelBuffer, 
                         std::vector<std::vector<color>>& frameBuffer, 
                         const int image_width, const int image_height,
                         const int samples_per_pixel, 
                         const camera& cam, const hittable_list& world) {

    const int max_depth = 10;
    for (auto& pixel : pixelBuffer) {
        color pixel_color(0,0,0);
        int j = pixel.first;
        int i = pixel.second;
        for (int s = 0; s < samples_per_pixel; s++) {
            auto u = double(i + random_double()) / (image_width - 1);
            auto v = double(j + random_double()) / (image_height - 1);

            ray r = cam.ray_at(u, v);
            pixel_color += ray_color(r, world, max_depth);
        }
        frameBuffer[j][i] = pixel_color;
    }   
}

int main()
{

    // window parameters
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 10;

    // camera
    //point3 lookfrom(10,7,8);
    point3 lookfrom(0.0f, 4.5f, 30.0f);
    point3 lookat(0.0f, 1.5f, 0.0f);
    vec3 vup(0,1,0);
    auto aperture = 0.1;
    auto dist_to_focus = (lookfrom - lookat).length();

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // world
    hittable_list world = teapot_scene();

    renderer r;
    r.set_scene(world);
    r.set_cam(cam);
    r.samples_per_pixel(samples_per_pixel);
    r.max_depth(max_depth);
    r.image_dims(image_width, image_height);
    r.num_threads(-1);  // choose for me

    r.render_scene();
    /*
    // render
    std::cout << "P3" << std::endl;
    std::cout << image_width << " " << image_height << std::endl;
    std::cout << "255" << std::endl;

    const int num_threads_supported = std::thread::hardware_concurrency();
    std::cerr << num_threads_supported << " threads supported " << std::endl;

    std::vector<std::vector<color>> frameBuffer(image_height, std::vector<color>(image_width));
    std::vector<std::vector<std::pair<int,int>>> pixelBuffer(num_threads_supported);

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {     
            pixelBuffer[j % num_threads_supported].push_back(std::make_pair(j,i));
        }
    }

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads_supported; t++) {
        threads.push_back(std::thread(compute_pixel_color, std::ref(pixelBuffer[t]), std::ref(frameBuffer), image_width, image_height, samples_per_pixel, std::ref(cam), std::ref(world)));
    }

    for (int t = 0; t < num_threads_supported; t++) {
        threads[t].join();
    }

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {      
            write_color(std::cout, frameBuffer[j][i], samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";*/
}