#pragma once

#include "default.hpp"

class Ray {
   public:
    Ray() {}
    ~Ray() {};
    Ray(vec3 pos, vec3 dir)
        : pos(pos), dir(dir) {}

    vec3 pos;
    vec3 dir;
};
