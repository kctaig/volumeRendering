#include <camera.hpp>
#include <film.hpp>
#include <volume.hpp>
#include "volumeRendering.hpp"
#include <fstream>

void writePointsToPLY(const std::string& filename, const std::vector<glm::vec3>& points) {
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing");
    }

    // д��PLY�ļ�ͷ
    file << "ply\n";
    file << "format ascii 1.0\n";
    file << "element vertex " << points.size() << "\n";
    file << "property float x\n";
    file << "property float y\n";
    file << "property float z\n";
    file << "end_header\n";

    // д�������
    for (const auto& point : points) {
        file << point.x << " " << point.y << " " << point.z << "\n";
    }

    file.close();
}


struct Quad {
    int v1, v2, v3, v4;
};


void generateOBJFromBoundingBox(const glm::vec3& minPoint, const glm::vec3& maxPoint, const std::string& filename) {
    // ����߽��İ˸�����
    glm::vec3 vertices[8] = {
        glm::vec3(minPoint.x, minPoint.y, minPoint.z),
        glm::vec3(maxPoint.x, minPoint.y, minPoint.z),
        glm::vec3(maxPoint.x, maxPoint.y, minPoint.z),
        glm::vec3(minPoint.x, maxPoint.y, minPoint.z),
        glm::vec3(minPoint.x, minPoint.y, maxPoint.z),
        glm::vec3(maxPoint.x, minPoint.y, maxPoint.z),
        glm::vec3(maxPoint.x, maxPoint.y, maxPoint.z),
        glm::vec3(minPoint.x, maxPoint.y, maxPoint.z)
    };

    // ����������
    std::vector<Quad> quads;
    quads.push_back({ 0, 1, 2, 3 }); // ǰ
    quads.push_back({ 4, 5, 6, 7 }); // ��
    quads.push_back({ 0, 4, 7, 3 }); // ��
    quads.push_back({ 1, 5, 6, 2 }); // ��
    quads.push_back({ 0, 1, 5, 4 }); // ��
    quads.push_back({ 3, 2, 6, 7 }); // ��

    // д��OBJ�ļ�
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    // д�붥��
    for (const auto& vertex : vertices) {
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }

    // д���棨OBJ�ļ��е����Ǵ�1��ʼ�����ģ�
    for (const auto& quad : quads) {
        file << "f "
            << quad.v1 + 1 << " " << quad.v2 + 1 << " " << quad.v3 + 1 << " "
            << quad.v4 + 1 << std::endl;
    }

    file.close();
}

int main() {
    // parse json and raw data
    Volume volume(
        "D:/Information/sci-vis/volumeRendering/data/cbct.raw",
        "D:/Information/sci-vis/volumeRendering/data/raw_file2.json");
    // set film
    Film film(volume.dimensions[0], volume.dimensions[2]);
    // set camera
    //vec3 center = vec3(volume.dimensions[0] * volume.spacing[0] / 2, volume.dimensions[1] * volume.spacing[1] / 2, volume.dimensions[2] * volume.spacing[2] / 2);
    vec3 pos = vec3(volume.dimensions[0] * volume.spacing[0] / 2, volume.dimensions[1] * volume.spacing[1] + 50, volume.dimensions[2] * volume.spacing[2] / 2);
    //vec3 up = vec3(volume.dimensions[0] * volume.spacing[0] / 2, volume.dimensions[1] * volume.spacing[1] + 50, -1);
    //Camera cam(pos, up, center);
	Camera cam(pos);
    cam.film = &film;
    renderVolume(cam, volume);

    //writePointsToPLY("D:/Information/sci-vis/volumeRendering/output/plane.ply", cam.plane);

	//generateOBJFromBoundingBox(volume.bbox.min, volume.bbox.max, "D:/Information/sci-vis/volumeRendering/output/bbox.obj");

    return 0;
}