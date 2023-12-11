#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "geometry.h"
using namespace std;

class Image
{
    vector<Vec3f> pixels;
    
  public:
    int width;
    int height;
    
    void set_pixel(int i, int j, const Vec3f colour)
    {
        pixels[i + j * width] = colour;
    }
    
    Vec3f get_pixel(int i, int j) const
    {
        return pixels[i + j * width];
    }
    
    void save(const string path)
    {
        ofstream ofs;
        ofs.open(path, ofstream::binary);
        
        ofs << "P6\n" << width << " " << height << "\n255\n";
        
        for (size_t i = 0; i < width * height; ++i)
        {
            for (size_t color = 0; color < 3; ++color)
            {
                float abcde;
                if(pixels[i][color]>1){
                    abcde=1.f;
                }
                else if(pixels[i][color]<0){
                    abcde=0.f;
                }
                else{
                    abcde=pixels[i][color];
                }
                ofs << (char)(255 * abcde);
            }
        }
        
        ofs.close(); 
    }
    
    Image() : width(0), height(0) { }
    
    Image(const int width, const int height) : width(width), height(height)
    {
        pixels = vector<Vec3f>(width * height);
    }
    
    Image(const string path)
    {
        ifstream file(path);
        if (file.fail())
        {
            cout << "Failed to open image at " << path << endl;
            return;
        }
        
        string format;
        int max_colour;
        file >> format >> width >> height >> max_colour;
        
        pixels = vector<Vec3f>(width * height);
        
        for (size_t i = 0; i < width * height; ++i)
        {
            int r, g, b;
            file >> r >> g >> b;
            Vec3f colour((float)r / max_colour, (float)g / max_colour, (float)b / max_colour);
            pixels[i] = colour;
        }
    }
};
