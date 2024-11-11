# pragma once

#include <camera.hpp>
#include <cmath>
#include <ray.hpp>
#include <volume.hpp>

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

    vec3 pos = cam.pos + ray.dir * cam.nearPlane;
    float stepSize = (cam.farPlane - cam.nearPlane) / cam.setp;
    for (float t = cam.nearPlane; t < cam.farPlane; t += stepSize) {

        float density = sampleVolume(volume, pos);
		vec3 sampleColor = volume.transfer(density);
        //density = (density - volume.range.x) / (volume.range.y - volume.range.x);
         
        //vec3 sampleColor = { 0,0,0 };
        //if (density < 0.3) sampleColor = { 0,0,1 };
        //else sampleColor = { 0,1,0 };
	
        //float sampleOpacity = density * 0.1f;            
        float sampleOpacity = (density  - volume.range.x) / (volume.range.y - volume.range.x);

        // Accumulate color and opacity using front-to-back compositing
        accumulatedColor += sampleColor * (1.0f - accumulatedOpacity);
        accumulatedOpacity += sampleOpacity * (1.0f - accumulatedOpacity);

        // Stop ray if fully opaque
        if (accumulatedOpacity >= 1.0f)
            break;

        // Advance along the ray
        pos = pos + ray.dir * stepSize;
    }
    return accumulatedColor;
}

// Main function to render an image using raycasting
void renderVolume(Camera& cam, const Volume& volume) {
    Film* film = cam.film;

//#pragma omp parallel for
    for (int y = 0; y < volume.dimensions[2]; ++y) {
        for (int x = 0; x < volume.dimensions[0]; ++x) {
			vec3 color = vec3{ 0.0f, 0.f, 0.0f };
            Ray ray = cam.generateRay(volume, glm::ivec2{x, y});
			if (cam.updatePlane(volume, ray))
                color = rayCast(cam, ray, volume);
				//color = vec3{ 1.0f, 0.0f, 0.0f };
			//cout << cam.nearPlane  << " " << cam.farPlane << endl;
			//cout << color.x << " " << color.y << " " << color.z << endl;
            film->setPixel(x, y, color);
        }
    }
    film->saveImage("D:/Information/sci-vis/volumeRendering/output/image.ppm");
    return;
}
