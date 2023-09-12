#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <cmath>
#include <vec3.h>

class transform {
    public:
        virtual vec3 apply(const vec3& input) const = 0;

        static vec3 apply_transforms(const vec3& input, const std::vector<std::shared_ptr<transform>>& transforms) {
            vec3 out = input;
            for (auto& transform_ptr : transforms) {
                out = transform_ptr->apply(out);
            }
            return out;
        }
    
    protected:
        transform(const vec3& params) : _params(params) {}
        vec3 _params;
};

class translation : public transform {
    public:
        translation(const vec3& params) : transform(params) {}
        translation(const float p1, const float p2, const float p3) : transform(vec3(p1,p2,p3)) {}

        vec3 apply(const vec3& input) const override {
            vec3 output = input + _params;
            return output;
        }
};

class scale : public transform {
    public:
        scale(const vec3& params) : transform(params) {}
        scale(const float p1, const float p2, const float p3) : transform(vec3(p1,p2,p3)) {}

        vec3 apply(const vec3& input) const override {
            vec3 output = input * _params;
            return output;
        }
};

class rotation : public transform {
    public:
        rotation(const vec3& params) : transform(params) {}
        rotation(const float p1, const float p2, const float p3) : transform(vec3(p1,p2,p3)) {}

        vec3 apply(const vec3& input) const override {
            vec3 output = rotateZ(rotateY(rotateX(input)));
            return output;
        }
    
    private:
        inline vec3 rotateX(const vec3& input) const {
            float thetaX = _params[0];
            vec3 output(input[0],
                        cos(thetaX) * input[1] - sin(thetaX) * input[2],
                        sin(thetaX * input[1] + cos(thetaX) * input[2]));
            return output;
        }

        inline vec3 rotateY(const vec3& input) const {
            float thetaY = _params[1];
            vec3 output(cos(thetaY) * input[0] + sin(thetaY) * input[2],
                        input[1],
                        -sin(thetaY) * input[0] + cos(thetaY) * input[2]);
            return output;
        }

        inline vec3 rotateZ(const vec3& input) const {
            float thetaZ = _params[2];
            vec3 output(cos(thetaZ) * input[0] - sin(thetaZ) * input[1],
                        sin(thetaZ) * input[0] + cos(thetaZ) * input[1],
                        input[2]);
            return output;
        }
};


#endif // TRANSFORM_H