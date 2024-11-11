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

    // 计算插值点在网格中的坐标
    vec3 gridPos = pos / s;

    // 获取网格坐标的整数部分和小数部分
    vec3 ijk0 = vec3(std::floor(gridPos.x), std::floor(gridPos.y), std::floor(gridPos.z));
    vec3 ijk1 = ijk0 + vec3(1.0f, 1.0f, 1.0f);

    // 限制索引在边界内
    ijk0.x = std::max(ijk0.x, 0.0f);
    ijk0.y = std::max(ijk0.y, 0.0f);
    ijk0.z = std::max(ijk0.z, 0.0f);
    ijk1.x = std::min(ijk1.x, (float)d[0] - 1.0f);
    ijk1.y = std::min(ijk1.y, (float)d[1] - 1.0f);
    ijk1.z = std::min(ijk1.z, (float)d[2] - 1.0f);

    // 获取插值点周围8个顶点的索引
    int x0 = static_cast<int>(ijk0.x);
    int y0 = static_cast<int>(ijk0.y);
    int z0 = static_cast<int>(ijk0.z);
    int x1 = static_cast<int>(ijk1.x);
    int y1 = static_cast<int>(ijk1.y);
    int z1 = static_cast<int>(ijk1.z);

    // 计算权重
    vec3 weights = gridPos - ijk0;

    // 提取8个顶点的密度值
    float c000 = volume.voxels[x0 + y0 * d[0] + z0 * d[0] * d[1]].density;
    float c100 = volume.voxels[x1 + y0 * d[0] + z0 * d[0] * d[1]].density;
    float c010 = volume.voxels[x0 + y1 * d[0] + z0 * d[0] * d[1]].density;
    float c110 = volume.voxels[x1 + y1 * d[0] + z0 * d[0] * d[1]].density;
    float c001 = volume.voxels[x0 + y0 * d[0] + z1 * d[0] * d[1]].density;
    float c101 = volume.voxels[x1 + y0 * d[0] + z1 * d[0] * d[1]].density;
    float c011 = volume.voxels[x0 + y1 * d[0] + z1 * d[0] * d[1]].density;
    float c111 = volume.voxels[x1 + y1 * d[0] + z1 * d[0] * d[1]].density;

    // 进行三线性插值
    float c00 = c000 * (1 - weights.x) + c100 * weights.x;
    float c01 = c001 * (1 - weights.x) + c101 * weights.x;
    float c10 = c010 * (1 - weights.x) + c110 * weights.x;
    float c11 = c011 * (1 - weights.x) + c111 * weights.x;

    float c0 = c00 * (1 - weights.y) + c10 * weights.y;
    float c1 = c01 * (1 - weights.y) + c11 * weights.y;

    float c = c0 * (1 - weights.z) + c1 * weights.z;

    return c;
}

// Perform raycasting for a single pixel
vec3 rayCast(const Camera& cam, const Ray& ray, const Volume& volume) {
    vec3 accumulatedColor = {0.0f, 0.0f, 0.0f};
    float accumulatedOpacity = 0.0f;

    vec3 pos = cam.pos;
    //float stepSize = (cam.farPlane - cam.nearPlane) / cam.setp;
    for (float t = 0; t < 500.f; t += 0.01f) {
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
    for (int y = 0; y < film->height; ++y) {
        for (int x = 0; x < film->width; ++x) {
			//vec3 color = { 0.0f, 0.0f, 0.0f };
            Ray ray = cam.generateRay(volume, glm::ivec2{x, y});
            vec3 color = rayCast(cam, ray, volume);
#if 0
            mtx.lock();
            count++;
            if (count % film->width == 0) {
                float progress = static_cast<float>(count) / (film->height * film->width);
                int percent = static_cast<int>(progress * 100); // 将进度转换为百分比
                cout << "Progress: " << percent << "%" << endl;
            }
            mtx.unlock();
#endif
            film->setPixel(x, y, color);
        }
    }
    film->saveImage("D:/Information/sci-vis/volumeRendering/output/image.ppm");
    return;
}
