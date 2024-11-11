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

vec3 Volume::transfer(float density) const
{
    // 定义颜色点
    std::vector<std::tuple<float, vec3>> color_points = {
        std::make_tuple(range.x, vec3{0.0f, 0.0f, 0.0f}),  // 空气，黑色
        std::make_tuple(200.0f, vec3{0.62f, 0.36f, 0.18f}),  // 软组织
        std::make_tuple(500.f, vec3{0.88f, 0.60f, 0.29f}),    // 骨骼阈值
        std::make_tuple(range.y, vec3{1.0f, 1.0f, 1.0f})     // 密集骨骼，亮白色
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

void Volume::updateBBox()
{
    bbox.min = vec3(0, 0, 0);
    bbox.max = vec3(dimensions[0] * spacing[0], dimensions[1] * spacing[1], dimensions[2] * spacing[2]);
}

void Volume::updateRange()
{
    range = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest());
    for (auto voxel : voxels) {
        if (voxel.density < range.x) range.x = (float)voxel.density;
        if (voxel.density > range.y) range.y = (float)voxel.density;
    }
}

Volume::Volume(const string& rawFilePath, const string& jsonFilePath) {
    json metadata = loadJson(jsonFilePath);
    vector<size_t>d = metadata["dimensions"];
    dimensions = d;
    auto s = metadata["spacing"];
	spacing = glm::vec3(s[0],s[1],s[2]);
    updateBBox();
    loadRawData(rawFilePath);
}

void Volume::loadRawData(const string& rawFilePath) {
    std::ifstream rawFile(rawFilePath, std::ios::binary);
    if (!rawFile.is_open()) {
        throw std::runtime_error("Failed to open raw data file.");
    }
    auto total = dimensions[0] * dimensions[1] * dimensions[2];
    voxels.resize(total);
    rawFile.read(reinterpret_cast<char*>(voxels.data()), total * sizeof(int16_t));
    updateRange();
}

glm::vec2 BBox::intersect(const Ray& ray) const
{
	float tmin = (min.x - ray.pos.x) / ray.dir.x;
	float tmax = (max.x - ray.pos.x) / ray.dir.x;

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (min.y - ray.pos.y) / ray.dir.y;
	float tymax = (max.y - ray.pos.y) / ray.dir.y;

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		return { -1, -1 };

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (min.z - ray.pos.z) / ray.dir.z;
	float tzmax = (max.z - ray.pos.z) / ray.dir.z;

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		return { -1, -1 };

	if (tzmin > tmin)
		tmin = tzmin;

	if (tzmax < tmax)
		tmax = tzmax;

	return { tmin, tmax };
}
