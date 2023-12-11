#pragma once
#include "objects.h"
#include "light.h"
#include <vector>

class Scene 
{
    vector<Object*> objects;
    vector<Light*> lights;
    
    bool scene_intersect(const Ray &ray, Vec3f &hit_point, Vec3f &hit_normal, Object* &hit_object) const
    {
        float best_t = numeric_limits<float>::max();
        float t = -1;
        bool did_intersect = false;
        Vec3f normal;
        
        for (auto obj: objects)
        {
            if (obj->ray_intersect(ray, t, normal))
            {
                if (t < best_t && t > 0)
                {
                    best_t = t;
                    hit_point = ray.origin + ray.direction * t;
                    hit_object = obj;
                    hit_normal = normal;
                    did_intersect = true;
                }
            }
        }
        
        return did_intersect;
    }
    
    bool is_light_obscured(const Light* light, const Vec3f hit_point, const Vec3f hit_normal) const
    {
        Vec3f light_dir = (light->position - hit_point).normalize();
        Vec3f light_origin = hit_point + light_dir * 0.001;
        Ray light_ray = Ray(light_origin, light_dir);
        
        Vec3f rec_hit_point;
        Vec3f rec_hit_normal;
        Object* rec_hit_object;        
        if (scene_intersect(light_ray, rec_hit_point, rec_hit_normal, rec_hit_object))
        {
            float light_dist = (light->position - hit_point).norm();
            float hit_dist = (rec_hit_point - hit_point).norm();
            if (hit_dist < light_dist)
                return true;
        }
        
        return false;
    }
    
    void process_lights(const Vec3f hit_point, const Vec3f hit_normal, Object* hit_object, const Ray ray, float &diff, float &spec) const
    {
        for (auto light : lights)
        {
            // za svaki izvor svjetlosti izracunati L
            if (is_light_obscured(light, hit_point, hit_normal))
            {
                continue;
            }
            
            // lambertov model
            // L = k_d * (I / r^2) * max(0, n * l)
            Vec3f l = (light->position - hit_point).normalize();
            float r = (light->position - hit_point).norm();
            
            float k_d = hit_object->material.diffuse_coef;
            float dist_factor = light->intensity / (r * r);
            float angle = hit_normal * l;
            float L = k_d * dist_factor * max(0.f, angle);
            
            diff += L;
            
            // blinn-phongov model
            // L = k_s * (I / r^2) * max(0, n * h)^p
            Vec3f h = (-ray.direction + l).normalize();
            float k_s = hit_object->material.specular_coef;
            float p = hit_object->material.phong_exp;
            
            L = k_s * dist_factor * pow(max(0.f, hit_normal * h), p);
            
            spec += L;
        }
    }
    
  public:
    Vec3f cast_ray(const Ray &ray) const
    {
        Vec3f hit_point;
        Vec3f hit_normal;
        Object* hit_object;
        
        if (!scene_intersect(ray, hit_point, hit_normal, hit_object))
        {
            return Vec3f(1.0, 1.0, 1.0);
        }
        else
        {

            Ray r2(hit_point + ray.direction * 0.001, ray.direction);
            Ray r3(hit_point + hit_normal * 0.001, hit_normal);
            float diff = 0;
            float spec = 0;

            process_lights(hit_point, hit_normal, hit_object, ray, diff, spec);

            return ((hit_object->material.diffuse_colour * diff + Vec3f(1, 1, 1) * spec) * (hit_object->material.opacity) + cast_ray(r2) * (1.f - hit_object->material.opacity)) * (1.f - hit_object->material.refl) +
                    cast_ray(r3) * hit_object->material.refl;
        }
    }
  
    void add_object(Object* object)
    {
        objects.push_back(object);
    }
    
    void add_light(Light* light)
    {
        lights.push_back(light);
    }
};