#include "default.hpp"
#include "loadRawFile.hpp"
#include "volumeRendering.hpp"
#include <camera.hpp>

int main() {

    auto [data, dimensions, spacing] = load_raw_data_with_meta(
        "D:/Information/sci-vis/volumeRendering/data/cbct.raw", 
        "D:/Information/sci-vis/volumeRendering/data/raw_file2.json"
        );

    // Generate synthetic volume data
    short3DVector volume = data;

    Camera cam((float)dimensions[0], (float)dimensions[1], (float)dimensions[0] + 50.f);
    // Render volume using raycasting
    int imageWidth = 128;
    int imageHeight = 128;
    renderVolume(cam,volume, imageWidth, imageHeight);

    return 0;
}