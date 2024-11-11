# pragma once

#include <camera.hpp>
#include <cmath>
#include <ray.hpp>
#include <volume.hpp>
#include <mutex>

std::mutex mtx;

// Sample the volume at a given point (trilinear interpolation for smooth values)
float sampleVolume(const Volume& volume, const vec3& pos) {
    vector<size_t> d = volume.dimensions;
    vec3 s = volume.spacing;
    int x = std::min(std::max(int(pos.x / s[0]), 0), (int)d[0] - 1);
    int y = std::min(std::max(int(pos.y / s[1]), 0), (int)d[1] - 1);
    int z = std::min(std::max(int(pos.z / s[2]), 0), (int)d[2] - 1);
	return (float)volume.voxels[x + y * d[0] + z * d[0] * d[1]].density;
}

// Perform raycasting for a single pixel
vec3 rayCast(const Camera& cam, const Ray& ray, const Volume& volume) {
    vec3 accumulatedColor = {0.0f, 0.0f, 0.0f};
    float accumulatedOpacity = 0.0f;

    vec3 pos = cam.pos;
    //float stepSize = (cam.farPlane - cam.nearPlane) / cam.setp;
    for (float t = 0; t < 400; t += 0.1) {
        pos = pos + ray.dir * t;
		if (!volume.insideBBox(pos))
			continue;
        float density = sampleVolume(volume, pos);
        density = (density - volume.range.x) / (volume.range.y - volume.range.x);
		glm::vec4 sampleColor = volume.transfer(density);
		float sampleOpacity = sampleColor.a;

        accumulatedColor += vec3(sampleColor) * (1.0f - accumulatedOpacity);
        accumulatedOpacity += sampleOpacity * (1.0f - accumulatedOpacity);

        if (accumulatedOpacity >= 1.0f)
            break;
       
    }
    return accumulatedColor;
}

// Main function to render an image using raycasting
void renderVolume(Camera& cam, const Volume& volume) {
    Film* film = cam.film;

    int count = 0;

#pragma omp parallel for num_threads(16)
    for (int y = 0; y < volume.dimensions[2]; ++y) {
        for (int x = 0; x < volume.dimensions[0]; ++x) {
            Ray ray = cam.generateRay(volume, glm::ivec2{x, y});
            vec3 color = rayCast(cam, ray, volume);
            mtx.lock();
            count++;
            if (count % volume.dimensions[0] == 0) {
               
                cout << (float)count / (volume.dimensions[0] * volume.dimensions[2]) << endl;
            }
            mtx.unlock();
            film->setPixel(x, y, color);
        }
    }
    film->saveImage("D:/Information/sci-vis/volumeRendering/output/image.ppm");
    return;
}
