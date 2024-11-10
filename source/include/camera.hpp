#pragma once

#include "film.hpp"
#include "ray.hpp"
#include <volume.hpp>

class Camera {
   public:
    Camera() {}

    Camera(vec3 pos, vec3 up, vec3 viewpoint, float vfov = 45)
        : pos(pos), up(up), viewPoint(viewPoint) {}

    bool updatePlane(const Volume& volume, const Ray& ray);

    Ray generateRay(const glm::ivec2& pixelCoord, const glm::vec2& offsets = {0.5, 0.5}) const;

    vec3 pos, up, viewPoint;
    Film* film;
    float nearPlane = -1, farPlane = -1, setp = 100;
    float vfov;
};
