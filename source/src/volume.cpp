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

glm::vec4 Volume::transfer(float density) const
{
    if (density > 0.14 && density <= 0.24) { // 皮肤
        return colors[0];
    }
    else if (density > 0.24 && density <= 0.3) { // 肌肉
        return colors[0];
    }
    else if (density > 0.3) { // 骨骼
        return colors[3];
    }
    else { // 其他
        return colors[0];
    }
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

bool Volume::insideBBox(const glm::vec3& point) const
{
    // 检查点是否在包围盒的范围内
    return (point.x >= bbox.min.x && point.x <= bbox.max.x) &&
        (point.y >= bbox.min.y && point.y <= bbox.max.y) &&
        (point.z >= bbox.min.z && point.z <= bbox.max.z);
}

Volume::Volume(const string& rawFilePath, const string& jsonFilePath) {
    json metadata = loadJson(jsonFilePath);
    vector<size_t>d = metadata["dimensions"];
    dimensions = d;
    auto s = metadata["spacing"];
	spacing = glm::vec3(s[0],s[1],s[2]);
	//spacing /= glm::vec3(dimensions[0], dimensions[1], dimensions[2]);
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
