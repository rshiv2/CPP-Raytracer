#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "renderer.h"

color renderer::ray_color(const ray& r, const hittable_list& world, const int depth) {

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

void renderer::thread_compute_pixel_colors(
    const std::vector<std::pair<int, int>>& perThreadPixelPos,
    std::vector<std::vector<color>>& frameBuffer) {

    for (auto& pixelPos : perThreadPixelPos) {
        color pixel_color(0,0,0);
        int j = pixelPos.first;
        int i = pixelPos.second;

        for (int s = 0; s < _samples_per_pixel; s++) {
            auto u = double(i + random_double()) / (_image_width - 1);
            auto v = double(j + random_double()) / (_image_height - 1);

            ray r = _cam.ray_at(u, v);
            pixel_color += ray_color(r, _scene, _max_depth);
        }
        frameBuffer[j][i] = pixel_color;
    }  

}

void renderer::write_color(const std::string& outputFile, const std::vector<std::vector<color>>& frameBuffer) const {

    std::cerr << "Writing data to disk" << std::endl;
    const int num_channels = 3;
    const float scale = 1.0f / _samples_per_pixel;

    uint8_t* data = new uint8_t[_image_width * _image_height * num_channels];
    // std::cerr << "Data size: " << _image_width * _image_height * num_channels;

    int index = 0;
    for (int j = _image_height - 1; j >= 0; --j) {
        for (int i = 0; i < _image_width; ++i) {

            auto pixelColor = frameBuffer[j][i];
            float r = pixelColor.x();
            float g = pixelColor.y();
            float b = pixelColor.z();

            r = std::sqrt(r * scale);
            g = std::sqrt(g * scale);
            b = std::sqrt(b * scale);

            int ir = int(255.99 * clamp(r, 0.0f, 0.999f));
            int ig = int(255.99 * clamp(g, 0.0f, 0.999f));
            int ib = int(255.99 * clamp(b, 0.0f, 0.999f));

            //std::cerr << index << ", " << data[index];
            data[index++] = ir;
            data[index++] = ig;
            data[index++] = ib;
        }
    }

    try {
        stbi_write_png(outputFile.c_str(), _image_width, _image_height, num_channels, data, _image_width * num_channels);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    delete[] data;
}

void renderer::render_scene() {
    
    std::vector<std::vector<color>> frameBuffer(_image_height, std::vector<color>(_image_width));
    std::vector<std::vector<std::pair<int,int>>> pixelBuffer(_nthreads);

    // assign pixels to each thread
    for (int j = _image_height - 1; j >= 0; --j) {
        for (int i = 0; i < _image_width; ++i) {     
            pixelBuffer[j % _nthreads].push_back(std::make_pair(j,i));
        }
    }

    // spawn threads
    std::vector<std::thread> threads;
    for (int t = 0; t < _nthreads; t++) {
        threads.push_back(std::thread([this, &pixelBuffer, &frameBuffer, t]() {
            this->thread_compute_pixel_colors(std::as_const(pixelBuffer[t]), frameBuffer);
        } ));
    }

    for (int t = 0; t < _nthreads; t++) {
        threads[t].join();
    }

    // write framebuffer to disk
    write_color(std::string("render.png"), frameBuffer);

    std::cerr << "\nDone.\n";
}