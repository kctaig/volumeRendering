#pragma once

#include <filesystem>
#include "default.hpp"

class Film {
   public:
    Film() {}
    Film(size_t width, size_t height)
        : width(width), height(height) {
        pixels.resize(width * height);
    }
    void saveImage(const std::filesystem::path& fileName);
    vec3 getPixel(size_t x, size_t y) { return this->pixels[y * width + x]; }
    void setPixel(size_t x, size_t y, const glm::vec3& color) { pixels[y * width + x] = color; }

    size_t width, height;
    vector<vec3> pixels;
};