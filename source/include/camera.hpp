# pragma once 

#include "default.hpp"
#include "vec.hpp"

class Camera
{
public:
 Camera() {}
 Camera(Vec3 pos)
     : pos(pos) {}
     Camera(float x,float y,float z){
         pos = Vec3(x, y, z);
     }
 Vec3 pos;

};

