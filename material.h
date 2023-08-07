#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

struct hit_record;

class material {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;

};

class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
                auto scatter_direction = rec.n + unit_vector(vec3::random_in_unit_sphere());

                if (scatter_direction.near_zero()) {
                    scatter_direction = rec.n;
                }

                scattered = ray(rec.p, scatter_direction, r_in.time());
                attenuation = albedo;
                return true;
            }

    private:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& a, double f) : albedo(a), fuzz(clamp(f, 0.0, 1.0)) {}

        virtual bool scatter(
            const ray& r_in ,const hit_record& rec, color& attenuation, ray& scattered) const override {
                vec3 scatter_direction = reflect(r_in.direction(), rec.n);

                scattered = ray(rec.p, scatter_direction + fuzz*vec3::random_in_unit_sphere(), r_in.time());
                attenuation = albedo;
                return dot(scattered.direction(), rec.n) > 0;
        }

    private:
        color albedo;
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double ir) : index_of_refraction(ir) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
                attenuation = color(1.0, 1.0, 1.0);
                double refraction_ratio = rec.front_face ? (1.0 / index_of_refraction) : index_of_refraction;

                double cos_theta = fmin(dot(-r_in.direction(), rec.n), 1.0);
                double sin_theta = sqrt(1 - cos_theta * cos_theta);
                bool cannot_refract = refraction_ratio * sin_theta > 1.0;
                vec3 direction;

                if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                    direction = reflect(r_in.direction(), rec.n);
                else 
                    direction = refract(r_in.direction(), rec.n, refraction_ratio);

                scattered = ray(rec.p, direction, r_in.time());
                return true;
            }
        
    private:
        double index_of_refraction;

    private:
        static double reflectance(double cosine, double ref_idx) {
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*std::pow((1 - cosine),5);
        }
};

#endif // MATERIAL_H