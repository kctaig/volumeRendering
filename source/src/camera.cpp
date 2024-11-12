#include <camera.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <fstream>

Ray Camera::generateRay(const Volume& volume ,const glm::ivec2& pixelCoord, const glm::vec2& offsets)  {
	float tanHalfVFOV = tan(glm::radians(vfov / 2));
    // the ratio of width to height
    auto aspectRatio = static_cast<float>(film->width) / static_cast<float>(film->height);
    // ndc coordinates
    auto ndc = (static_cast<glm::vec2>(pixelCoord) + offsets) / glm::vec2(film->width - 1, film->height - 1);
    ndc.y = 1.f - ndc.y;
    ndc = ndc * 2.f - 1.f;
    ndc.x *= tanHalfVFOV * aspectRatio;
    ndc.y *= tanHalfVFOV;
    // Adjust the view plane distance for field of view
    float viewPlaneZ = -1.0f / tanHalfVFOV; 
    // Set up view plane in camera space
    glm::vec4 worldCoord = glm::vec4(ndc, viewPlaneZ, 1.0f);
    // from camera space to world space
    worldCoord = glm::inverse(glm::lookAt(pos, viewPoint, up)) * worldCoord;
    auto rayDir = glm::normalize(vec3(worldCoord)-pos);
    return Ray(pos, rayDir);
}
