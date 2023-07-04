#pragma once


#include "global.hpp"
#include "math.hpp"
#include <string>

class Texture {
public:
    u8* mData;
    s32 mWidth;
    s32 mHeight;

    Texture();
    Texture(std::string path);
    ~Texture();
    Math::v3 GetColor(r32 u, r32 v);
};
