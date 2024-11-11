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
    glm::vec4 transfer(float density) const;
    void updateBBox();
    void updateRange();
    bool insideBBox(const glm::vec3& point) const;

    vector<size_t> dimensions;
    vec3 spacing;
    vector<Voxel> voxels;
    BBox bbox;
    glm::vec2 range;

    glm::vec4 colors[4] = {
        glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),   // ÆäËû
        //glm::vec4(0.01f, 0.08f, 0.06f, 0.01f), // Æ¤·ô
		glm::vec4(0.08,0.06,0.05,0.05), // Æ¤·ô
        glm::vec4(0.07f, 0.01f, 0.01f, 0.02f), // ¼¡Èâ
        glm::vec4(0.1f, 0.1f, 0.1f, 0.05f)    // ¹Ç÷À
    };
};                   