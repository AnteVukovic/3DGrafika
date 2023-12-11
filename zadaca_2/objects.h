#pragma once
#include <cmath>
#include "geometry.h"
#include "ray.h"
#include "material.h"

using namespace std;
class Object 
{
  public:
    Material material;
    virtual bool ray_intersect(const Ray &ray, float &t, Vec3f &normal) const = 0;  
};

class Sphere : public Object
{
    float r;
    Vec3f c;
      
  public:
    Sphere(const Vec3f &c, const float &r) : c(c), r(r) { }
    Sphere(const Vec3f &c, const float &r, const Material &mat) : c(c), r(r)
    { 
        Object::material = mat;
    }

    bool ray_intersect(const Ray &ray, float &t, Vec3f &normal) const
    {
        Vec3f e_minus_c = ray.origin - c;
        float d2 = ray.direction * ray.direction;
        
        float disc = pow(ray.direction * e_minus_c, 2) - d2 * (e_minus_c * e_minus_c - r * r);
        
        if (disc < 0)
        {
            return false;
        }
        else
        {
            bool ray_inside_sphere = e_minus_c * e_minus_c <= r * r;
            if (ray_inside_sphere)
            {
                float t1 = (-ray.direction * e_minus_c + sqrt(disc)) / d2;
                t = t1;
            }
            else
            {
                float t2 = (-ray.direction * e_minus_c - sqrt(disc)) / d2;
                t = t2;
            }
            
            Vec3f hit_point = ray.origin + ray.direction * t;
            normal = (hit_point - c).normalize();
            return true;
        }
    }
};


Vec3f cross(const Vec3f & a, const Vec3f & b) {
    return Vec3f(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}

struct Triangle : Object
{

    Vec3f a, b, c;

    Triangle(const Vec3f &a, const Vec3f &b, const Vec3f &c, const Material &m) : a(a), b(b), c(c)
    {
        Object::material = m;
    }

    bool ray_intersect(const Ray &ray, float &t, Vec3f &normal) const
    {
        Vec3f p=ray.origin;
        Vec3f d=ray.direction;

        float detA = determinant(a - b, a - c, d);
        float beta = determinant(a - p, a - c, d) / detA;
        float gamma = determinant(a - b, a - p, d) / detA;
        float alpha = 1 - beta - gamma;

        float t0 = determinant(a - b, a - c, a - p) / detA;
        if (t0 < 0)
            return false;


        if (std::min(alpha, std::min(beta, gamma)) >= 0 && std::max(alpha, std::max(beta, gamma)) <= 1)
        {
            t = t0;
            normal = cross(b - a, c - a).normalize();
            return true;
        }
        else
            return false;
    }

    float ray_intersect2(const Ray &ray, float &t, Vec3f &normal) const
    {
        Vec3f p=ray.origin;
        Vec3f d=ray.direction;

        float detA = determinant(a - b, a - c, d);
        float beta = determinant(a - p, a - c, d) / detA;
        float gamma = determinant(a - b, a - p, d) / detA;
        float alpha = 1 - beta - gamma;

        float t0 = determinant(a - b, a - c, a - p) / detA;
        t = t0;
        return t;
    }
};

bool cmp(const Triangle &tr1, const Triangle &tr2, const Vec3f &point){
    Vec3f v1((tr1.a[0]+tr1.b[0]+tr1.c[0])/3.0, (tr1.a[1]+tr1.b[1]+tr1.c[1])/3.0, (tr1.a[2]+tr1.b[2]+tr1.c[2])/3.0);
    Vec3f v2((tr2.a[0]+tr2.b[0]+tr2.c[0])/3.0, (tr2.a[1]+tr2.b[1]+tr2.c[1])/3.0, (tr2.a[2]+tr2.b[2]+tr2.c[2])/3.0);
    float d1 = sqrt(pow(v1[0]-point[0], 2) + pow(v1[1]-point[1], 2) + pow(v1[2]-point[2], 2));
    float d2 = sqrt(pow(v2[0]-point[0], 2) + pow(v2[1]-point[1], 2) + pow(v2[2]-point[2], 2));
    return d1<d2;
}

class Cuboid : public Object
{
    Vec3f mini, maxi, centre;
public:
    Vec3f to1 = Vec3f(mini[0], mini[1], mini[2]);
    Vec3f to2 = Vec3f(maxi[0], mini[1], mini[2]);
    Vec3f to3 = Vec3f(mini[0], maxi[1], mini[2]);
    Vec3f to4 = Vec3f(maxi[0], maxi[1], mini[2]);

    Vec3f to5 = Vec3f(mini[0], mini[1], maxi[2]);
    Vec3f to6 = Vec3f(maxi[0], mini[1], maxi[2]);
    Vec3f to7 = Vec3f(mini[0], maxi[1], maxi[2]);
    Vec3f to8 = Vec3f(maxi[0], maxi[1], maxi[2]);

    vector<Triangle> triangles;

    Cuboid(const Vec3f &mini, const Vec3f &maxi) : mini(mini), maxi(maxi) { centre = Vec3f((maxi[0]+mini[0])/2, (maxi[1]+mini[1])/2, (maxi[1]+mini[1])/2); }
    Cuboid(const Vec3f &mini, const Vec3f &maxi, const Material &mat) : mini(mini), maxi(maxi)
    {
        Object::material = mat;
        triangles.push_back(Triangle(to1, to2, to6, mat));
        triangles.push_back(Triangle(to1, to5, to6, mat));

        triangles.push_back(Triangle(to1, to2, to4, mat));
        triangles.push_back(Triangle(to1, to3, to4, mat));

        triangles.push_back(Triangle(to1, to5, to7, mat));
        triangles.push_back(Triangle(to1, to3, to7, mat));

        triangles.push_back(Triangle(to6, to2, to4, mat));
        triangles.push_back(Triangle(to8, to4, to6, mat));

        triangles.push_back(Triangle(to8, to3, to4, mat));
        triangles.push_back(Triangle(to8, to3, to7, mat));

        triangles.push_back(Triangle(to8, to6, to5, mat));
        triangles.push_back(Triangle(to8, to7, to5, mat));

        centre = Vec3f((maxi[0]+mini[0])/2, (maxi[1]+mini[1])/2, (maxi[1]+mini[1])/2);
    }

    bool ray_intersect(const Ray &ray, float &t, Vec3f &normal) const
    {
        vector<Triangle> triangles2;
        for(auto i:triangles){
            if(i.ray_intersect(ray, t, normal)){
                triangles2.push_back(i);
            }
        }
        if(triangles2.empty()){
            return false;
        }
        else{
            Vec3f ori = ray.origin;
            sort(triangles2.begin(), triangles2.end(), [ori](const Triangle &t1, const Triangle &t2){
                return cmp(t1, t2,ori);
            });
            t=triangles2[0].ray_intersect2(ray, t, normal);
            Vec3f hit_point = ray.origin + ray.direction * t;
            normal = (centre - hit_point).normalize();/*cross(triangles2[0].b - triangles2[0].a, triangles2[0].c - triangles2[0].a).normalize();*/
            return true;
        }
    }
};