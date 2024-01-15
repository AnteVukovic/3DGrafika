#include <iostream>
#include <cmath>
#include "tgaimage.h"
using namespace std;

int main()
{
    // definiraj sliku
    TGAImage image(width, height, TGAImage::RGB);
    
    // nacrtaj nekoliko duzina    
    draw_line(10, 20, 180, 60, image, white);
    draw_line(180, 80, 10, 40, image, white);
    
    //draw_line(20, 180, 140, 170, image, red);
    //draw_line(80, 40, 110, 120, image, blue);
    
    // spremi sliku 
    image.flip_vertically();
    image.write_tga_file("lines.tga");
}