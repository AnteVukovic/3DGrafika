#pragma once
#include "geometry.h"

class Material
{
  public:
    Vec3f diffuse_colour;
    float diffuse_coef = 1;
    
    float specular_coef = 1;
    float phong_exp = 1000;

    float opacity;
    float refl;
    
    Material() : diffuse_colour(Vec3f(0, 0, 0)), opacity(1.f), refl(0.f) { }
    Material(const Vec3f &colour, float opacity, float refl) : diffuse_colour(colour), opacity(opacity), refl(refl) { }
};