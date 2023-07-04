#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :mData(nullptr), mWidth(-1), mHeight(-1) {}
Texture::Texture(std::string path) {
    int n;
    mData = stbi_load(path.c_str(), &mWidth, &mHeight, &n, 3);
}
Texture::~Texture() {
    if (mData) {
        stbi_image_free(mData);
    }
}
Math::v3 Texture::GetColor(r32 u, r32 v) {

    if (u > 1) {
        u = u - (s32)u;
    }
    if (v > 1) {
        v = v - (s32)v;
    }

    int x = u * mWidth;
    int y = v * mHeight;
    return RGB::BufferToR32V3(&mData[x * 3 + y * mWidth * 3]);
}

