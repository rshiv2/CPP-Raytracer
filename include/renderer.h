#ifndef RENDERER_H
#define RENDERER_H

#include "rtcore.h"

class renderer {

    public:
        renderer() : _nthreads(std::thread::hardware_concurrency()) {}

        void render_scene();

        void set_scene(const hittable_list& scene) {
            _scene = scene;
        }

        void set_cam(const camera& cam) {
            _cam = cam;
        }

        void samples_per_pixel(const unsigned int s) {
            _samples_per_pixel = s;
        }

        void max_depth(const unsigned int d) {
            _max_depth = d;
        }

        void image_dims(const unsigned int width, const unsigned int height) {
            _image_width = width;
            _image_height = height;
        }

        void num_threads(const int t) {
            if (t < 0) {
                _nthreads = std::thread::hardware_concurrency();
            } else {
                _nthreads = std::min((int)std::thread::hardware_concurrency(), t);
            }
        }

    private:

        void thread_compute_pixel_colors(const std::vector<std::pair<int, int>>& perThreadPixelPos,
                                  std::vector<std::vector<color>>& frameBuffer);
        
        color ray_color(const ray& r, const hittable_list& world, const int depth);

        void write_color(const std::string& outputFile, const std::vector<std::vector<color>>& frameBuffer) const;

    private:
        hittable_list _scene;
        camera _cam;
        unsigned int _samples_per_pixel;
        unsigned int _max_depth;
        unsigned int _image_width;
        unsigned int _image_height;


        unsigned int _nthreads;

};

#endif // RENDERER_H