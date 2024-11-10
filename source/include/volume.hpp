#pragma once

#include "nlohmann/json.hpp"
#include <ray.hpp>

using json = nlohmann::json;

class Voxel {
   public:
    int16_t density;
};

class BBox {
public:

	BBox() {}
	BBox(vec3 min, vec3 max) : min(min), max(max) {}

    glm::vec2 intersect(const Ray& ray) const;

	vec3 min;
	vec3 max;
};

class Volume {
   public:
    Volume() {}
    ~Volume() {}
    Volume(const string& rawFilePath, const string& jsonFilePath);
    void loadRawData(const string& rawFilePath);
    json loadJson(const string& jsonFilePath);
    void preProcessVolume();
    vec3 transfer(float density) const;

    vector<size_t> dimensions;
    vec3 spacing;
    vector<Voxel> voxels;
    BBox bbox;
};