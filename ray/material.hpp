#pragma once

#include "math.hpp"

class Texture;

struct Material {
    bool hasAlbedoTexture;
    Texture* albedoTexture;
    Math::v3 albedo;

    r32 roughness;

    bool hasEmission;
    Math::v3 emission;
};
