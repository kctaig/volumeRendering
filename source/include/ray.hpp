#pragma once

#include "default.hpp"
#include "vec.hpp"
class ray
{

public:
    ray(){}
    ~ray(){};
    ray(Vec3 pos, Vec3 dir):pos(pos),dir(dir){}

    Vec3 pos;
    Vec3 dir;
};

