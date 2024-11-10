#include <camera.hpp>
#include <film.hpp>
#include <volume.hpp>
#include "volumeRendering.hpp"

int main() {
    // parse json and raw data
    Volume volume(
        "D:/Information/sci-vis/volumeRendering/data/cbct.raw",
        "D:/Information/sci-vis/volumeRendering/data/raw_file2.json");
    // set film
    Film film(128, 128);
    // set camera
    vec3 center = vec3(volume.dimensions[0] * volume.spacing[0] / 2, volume.dimensions[1] * volume.spacing[1] / 2, volume.dimensions[2] * volume.spacing[2] / 2);
    //vec3 pos = vec3(volume.dimensions[0] * volume.spacing[0] / 2, volume.dimensions[1] * volume.spacing[1] / 2, -1);
    vec3 pos = center * 2.f + 10.f;
    Camera cam(pos, {0, 1, 0}, center);

    cam.film = &film;
    renderVolume(cam, volume);

    return 0;
}