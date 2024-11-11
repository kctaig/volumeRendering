#pragma once

#include "film.hpp"
#include "ray.hpp"
#include <volume.hpp>

class Camera {
   public:
    Camera() {}

    Camera(vec3 pos, vec3 up, vec3 viewpoint, float vfov = 90)
        : pos(pos), up(up), viewPoint(viewPoint) {}

    Camera(vec3 pos):pos(pos){}
    Ray generateRay(const Volume& volume, const glm::ivec2& pixelCoord, const glm::vec2& offsets = {0.5, 0.5});

    vec3 pos, up, viewPoint;
    Film* film;
    float vfov;
};
