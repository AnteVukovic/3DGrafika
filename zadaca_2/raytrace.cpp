#include "image.h"
#include "geometry.h"
#include "ray.h"
#include "objects.h"
#include "scene.h"
#include "light.h"
#include <cmath>
using namespace std;

// compile with: g++ -std=c++17 raytrace.cpp -o out.exe

Ray ray_to_pixel(Vec3f origin, const Image &img, int i, int j)
{
    Ray ray = Ray();

    float aspect_ratio = (float)img.width / img.height;
 
    float x = (-1 + 2 * (i + 0.5) / (float)img.width) * aspect_ratio;
    float y = (1 - 2 * (j + 0.5) / (float)img.height);
    float z = -1;

    ray.origin = origin;
    ray.direction = Vec3f(x, y, z).normalize();
    return ray;
}

void render(const Scene &scene)
{
    // dimenzije slike
    const int width = 1024;
    const int height = 768;
    
    Vec3f origin = Vec3f(0, 0, 0);
    
    Image img = Image(width, height);
    
    // crtanje slike
    for (size_t j = 0; j < height; ++j)
    {
        for (size_t i = 0; i < width; ++i)
        {
            Ray ray = ray_to_pixel(origin, img, i, j);
            Vec3f colour = scene.cast_ray(ray);
            img.set_pixel(i, j, colour);
        }
    }
    
    img.save("./img.ppm");
}

int main()
{
    Scene scene;
    
    Material red04(Vec3f(1, 0, 0), 0.4, 0.6);
    Material green03(Vec3f(0, 1, 0), 0.3, 0.3);
    Material blue06(Vec3f(0, 0, 1), 0.6, 0.4);
    Material grey09(Vec3f(0.5, 0.5, 0.5), 0.9, 0.5);
    Material purple08(Vec3f(1, 0, 1), 0.8, 0.8);
    Material yellow02(Vec3f(1, 1, 0), 0.8, 0.3);
    
    Sphere s1(Vec3f(-3, 0, -16), 2, red04);
    Sphere s2(Vec3f(-1, -1.5, -12), 2.1, green03);
    Sphere s3(Vec3f(1.5, -0.5, -18), 3, blue06);
    Sphere s4(Vec3f(7, 5, -18), 4, grey09);

    Cuboid c1(Vec3f(5, -5, -10), Vec3f(7, -2, -11), purple08);
    Cuboid c2(Vec3f(-10, -6, -23), Vec3f(-6, -2, -25), yellow02);

    scene.add_object(&s1);
    scene.add_object(&s2);
    scene.add_object(&s3);
    scene.add_object(&s4);
    scene.add_object(&c1);
    scene.add_object(&c2);
    
    Light l1(Vec3f(-20, 20, 20), 1500);
    Light l2(Vec3f(20, 30, 20), 1800);
    
    scene.add_light(&l1);
    scene.add_light(&l2);
    
    render(scene);
    return 0;
}