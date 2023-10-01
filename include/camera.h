#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:

        camera() = default;

        camera(point3 lookfrom,
               point3 lookat,
               vec3 vup,
               double vfov,
               double aspect_ratio,
               double aperture,
               double focus_dist,
               double _time0 = 0.0,
               double _time1 = 0.0) {

            auto viewport_height = 2.0 * tan(degrees_to_radians(vfov/2));
            auto viewport_width = viewport_height * aspect_ratio;
            auto focal_length = 1.0;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);
    
            origin = lookfrom; 
            horizontal = viewport_width * u * focus_dist;
            vertical = viewport_height * v * focus_dist;
            lower_left_corner = origin - horizontal / 2 - vertical / 2 - w * focus_dist;
        
            lens_radius = aperture / 2; 

            time0 = _time0;
            time1 = _time1;
        }

        ray ray_at(const double s, const double t) const {
            vec3 rd = lens_radius * vec3::random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            return ray(origin + offset, 
                        lower_left_corner + s*horizontal + t*vertical - origin - offset,
                        random_double(time0, time1));
        }

    private:
        point3 origin;
        vec3 horizontal;
        vec3 vertical;
        point3 lower_left_corner;

        vec3 u, v, w;
        double lens_radius;

        double time0, time1;
};


#endif // CAMERA_H