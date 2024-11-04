# pragma once
#include <cmath>
#include "default.hpp"
#include "vec.hpp"

// Define 3D volume dimensions
const int WIDTH = 128;
const int HEIGHT = 128;
const int DEPTH = 128;

// Define camera properties
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 100.0f;
const float STEP_SIZE = 0.01f;  // Step size for ray marching


// Sample the volume at a given point (trilinear interpolation for smooth values)
float sampleVolume(const short3DVector& volume, const Vec3& pos) {
    int x = std::min(std::max(int(pos.x), 0), WIDTH - 1);
    int y = std::min(std::max(int(pos.y), 0), HEIGHT - 1);
    int z = std::min(std::max(int(pos.z), 0), DEPTH - 1);
    return volume[x][y][z];
}

// Compute ray direction for each pixel (for orthogonal projection here)
Vec3 computeRayDirection(const Vec3& cameraPos, int pixelX, int pixelY, int imageWidth, int imageHeight) {
    float u = (float(pixelX) / imageWidth) * 2.0f - 1.0f;
    float v = (float(pixelY) / imageHeight) * 2.0f - 1.0f;
    return { u, v, -1.0f };  // Assuming camera looks along -Z
}

// Perform raycasting for a single pixel
Vec3 rayCast(const Vec3& cameraPos, const Vec3& rayDir, const short3DVector& volume) {
    Vec3 accumulatedColor = { 0.0f, 0.0f, 0.0f };
    float accumulatedOpacity = 0.0f;

    // Ray starting point (move into the volume)
    Vec3 pos = cameraPos;
    for (float t = NEAR_PLANE; t < FAR_PLANE; t += STEP_SIZE) {
        // Sample volume and compute color and opacity
        float density = sampleVolume(volume, pos);
        Vec3 sampleColor = { density, density, density };  // Grayscale color based on density
        float sampleOpacity = density * 0.1f;  // Adjust opacity scaling factor

        // Accumulate color and opacity using front-to-back compositing
        accumulatedColor = accumulatedColor + sampleColor * sampleOpacity * (1.0f - accumulatedOpacity);
        accumulatedOpacity += sampleOpacity * (1.0f - accumulatedOpacity);

        // Stop ray if fully opaque
        if (accumulatedOpacity >= 1.0f) break;

        // Advance along the ray
        pos = pos + rayDir * STEP_SIZE;
    }

    return accumulatedColor;
}

// Main function to render an image using raycasting
void renderVolume(const Camera& cam, const short3DVector& volume, int imageWidth, int imageHeight) {
    Vec3 cameraPos = cam.pos;
    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            Vec3 rayDir = computeRayDirection(cameraPos, x, y, imageWidth, imageHeight);
            Vec3 color = rayCast(cameraPos, rayDir, volume);
            std::cout << "(" << color.x << ", " << color.y << ", " << color.z << ") ";
        }
        std::cout << std::endl;
    }
}


