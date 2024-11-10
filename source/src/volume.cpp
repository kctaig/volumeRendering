#include "volume.hpp"
#include <fstream>

json Volume::loadJson(const string& jsonFilePath) {
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        throw std::runtime_error("Failed to open jsonData file.");
    }
    json jsonData;
    jsonFile >> jsonData;
    return jsonData;
}

void Volume::preProcessVolume()
{
    int16_t maxDensity = std::numeric_limits<int16_t>::lowest();
    int16_t minDensity = std::numeric_limits<int16_t>::max();

    // 寻找密度值的最小和最大值
    for (auto& voxel : voxels) {
        if (voxel.density > maxDensity) maxDensity = voxel.density;
        if (voxel.density < minDensity) minDensity = voxel.density;
    }

    // 归一化密度值到[0, 1]区间
    for (auto& voxel : voxels) {
        voxel.density = (voxel.density - minDensity) / (maxDensity - minDensity);
    }
}

vec3 Volume::transfer(float density) const
{
    // 定义颜色点
    std::vector<std::tuple<float, vec3>> color_points = {
        std::make_tuple(-3024.0f, vec3{0.0f, 0.0f, 0.0f}),  // 空气，黑色
        std::make_tuple(-800.0f, vec3{0.62f, 0.36f, 0.18f}),  // 软组织
        std::make_tuple(0.0f, vec3{0.88f, 0.60f, 0.29f}),    // 骨骼阈值
        std::make_tuple(3071.0f, vec3{1.0f, 1.0f, 1.0f})     // 密集骨骼，亮白色
    };

    // 在颜色点之间进行线性插值
    for (size_t i = 0; i < color_points.size() - 1; ++i) {
        float point_density;
        vec3 point_color;
        std::tie(point_density, point_color) = color_points[i];
        float next_point_density;
        vec3 next_point_color;
        std::tie(next_point_density, next_point_color) = color_points[i + 1];

        if (point_density <= density && density <= next_point_density) {
            // 计算插值比例
            float alpha = (density - point_density) / (next_point_density - point_density);
            // 计算并返回插值后的颜色
            return vec3{
                point_color.r + alpha * (next_point_color.r - point_color.r),
                point_color.g + alpha * (next_point_color.g - point_color.g),
                point_color.b + alpha * (next_point_color.b - point_color.b)
            };
        }
    }

    // 如果密度值超出定义的颜色点范围，返回默认颜色（这里假设为黑色）
    return vec3{ 0.0f, 0.0f, 0.0f };
}

Volume::Volume(const string& rawFilePath, const string& jsonFilePath) {
    json metadata = loadJson(jsonFilePath);
    vector<size_t>d = metadata["dimensions"];
    dimensions = d;
    auto s = metadata["spacing"];
	spacing = glm::vec3(s[0],s[1],s[2]);
    loadRawData(rawFilePath);
	//preProcessVolume();
	bbox.min = vec3(0, 0, 0);
	bbox.max = vec3(dimensions[0] * spacing[0], dimensions[1] * spacing[1], dimensions[2] * spacing[2]);
}

void Volume::loadRawData(const string& rawFilePath) {
    std::ifstream rawFile(rawFilePath, std::ios::binary);
    if (!rawFile.is_open()) {
        throw std::runtime_error("Failed to open raw data file.");
    }
    auto total = dimensions[0] * dimensions[1] * dimensions[2];
    voxels.resize(total);
    rawFile.read(reinterpret_cast<char*>(voxels.data()), total * sizeof(int16_t));
}

glm::vec2 BBox::intersect(const Ray& ray) const
{
    vec3 invDir = vec3(1.0f) / ray.dir;
    vec3 tMin = (min - ray.pos) * invDir;
    vec3 tMax = (max - ray.pos) * invDir;
    vec3 t1 = glm::min(tMin, tMax);
    vec3 t2 = glm::max(tMin, tMax);
    float tNear = std::max(std::max(t1.x, t1.y), t1.z);
    float tFar = std::min(std::min(t2.x, t2.y), t2.z);
    if (tNear < 0 || tNear > tFar) {
        return glm::vec2(-1.f, -1.f);
    }
    return glm::vec2(tNear, tFar);
}
