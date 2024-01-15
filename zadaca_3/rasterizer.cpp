#include <iostream>
#include <cmath>
#include <vector>
#include "tgaimage.h"
using namespace std;

// g++ rasterizer.cpp tgaimage.cpp -o out.exe

// dimenzije slike
const int width  = 512;
const int height = 512;

// definirajmo boje
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0, 0, 255);
const TGAColor blue  = TGAColor(0, 0, 255, 255);
const TGAColor green   = TGAColor(0, 255, 0, 255);
const TGAColor purple  = TGAColor(255, 0, 255, 255);
const TGAColor yellow  = TGAColor(255, 255, 0, 255);
const TGAColor cyan  = TGAColor(0, 255, 255, 255);
const TGAColor orange  = TGAColor(255, 127, 0, 255);


void solve(vector<int>& j1, vector<int>& j2, vector<int>& j3, vector<int>& constants, vector<float>& solution) {
    vector< vector<int> > coefficients;
    coefficients.push_back(j1);
    coefficients.push_back(j2);
    coefficients.push_back(j3);
    vector< vector<float> > coeffsFloat(3, vector<float>(3));
    vector<float> constantsFloat(3);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            coeffsFloat[i][j] = (float)(coefficients[i][j]);
        }
        constantsFloat[i] = (float)(constants[i]);
    }

    for (int i = 0; i < 3; i++) {
        float divisor = coeffsFloat[i][i];
        for (int j = i; j < 3; j++) {
            coeffsFloat[i][j] /= divisor;
        }
        constantsFloat[i] /= divisor;

        for (int k=0; k<3; k++) {
            if (k!=i) {
                double factor = coeffsFloat[k][i];
                for (int j=i; j<3; j++) {
                    coeffsFloat[k][j] -= factor * coeffsFloat[i][j];
                }
                constantsFloat[k] -= factor * constantsFloat[i];
            }
        }
    }
    solution = constantsFloat;
}

void set_color(int x, int y, TGAImage &image, TGAColor color, bool invert = false)
{
    image.set(y, x, color);    
}

float line(float x0, float y0, float x1, float y1, float x, float y)
{
    return (y0 - y1) * x + (x1 - x0) * y + x0 * y1 - x1 * y0;
}

void line_naive(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    for (float t = 0; t < 1; t += 0.01)
    {
        int x = x0 * (1.0f - t) + x1 * t;
        int y = x0 * (1.0f - t) + y1 * t;
        set_color(x, y, image, color);
    }
}

void line_midpoint(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color, bool invert)
{
    int y = y0;
    int d = line(x0, y0, x1, y1, x0 + 1, y0 + 0.5);
    
    int dx = (x1 - x0);
    int dy = (y1 - y0);
    int increment = 1;
    
    if (dy < 0)
    {
        // pravac ide od gore prema dolje
        dy = -dy;
        increment = -increment;
    }
    
    for (int x = x0; x <= x1; ++x)
    {
        if (invert)
        {
            set_color(x, y, image, color);
        }
        else
        {
            set_color(y, x, image, color);       
        }
        
        if (d < 0)
        {
            y = y + increment;
            d = d + dx - dy;
        }
        else
        {
            d = d - dy;
        }
    }
}

void draw_triangle_2d(vector<int> x, vector<int> y, vector<int> z, TGAImage &image, TGAColor color){
    vector<int> a;
    vector<int> b;
    vector<int> c;
    a.push_back(x[0]);
    a.push_back(y[0]);
    a.push_back(z[0]);
    b.push_back(x[1]);
    b.push_back(y[1]);
    b.push_back(z[1]);
    c.push_back(1);
    c.push_back(1);
    c.push_back(1);
    for(int i=min(x[0], min(y[0], z[0])); i<=max(x[0], max(y[0], z[0])); i++){
        for(int j=min(x[1], min(y[1], z[1])); j<=max(x[1], max(y[1], z[1])); j++){
            vector<int> r;
            r.push_back(i);
            r.push_back(j);
            r.push_back(1);
            vector<float> sol;
            solve(a, b, c, r, sol);
            if(sol[0]>=0 && sol[0]<=1 && sol[1]>=0 && sol[1]<=1 && sol[2]>=0 && sol[2]<=1){
                set_color(i, j, image, color);
            }
        }
    }
}

void draw_triangle_2d_gouraud(vector<int> x, vector<int> y, vector<int> z, TGAImage &image, TGAColor color1, TGAColor color2, TGAColor color3){
    vector<int> a;
    vector<int> b;
    vector<int> c;
    a.push_back(x[0]);
    a.push_back(y[0]);
    a.push_back(z[0]);
    b.push_back(x[1]);
    b.push_back(y[1]);
    b.push_back(z[1]);
    c.push_back(1);
    c.push_back(1);
    c.push_back(1);
    for(int i=min(x[0], min(y[0], z[0])); i<=max(x[0], max(y[0], z[0])); i++){
        for(int j=min(x[1], min(y[1], z[1])); j<=max(x[1], max(y[1], z[1])); j++){
            vector<int> r;
            r.push_back(i);
            r.push_back(j);
            r.push_back(1);
            vector<float> sol;
            solve(a, b, c, r, sol);
            if(sol[0]>=0 && sol[0]<=1 && sol[1]>=0 && sol[1]<=1 && sol[2]>=0 && sol[2]<=1){
                int r=floor(sol[0]*color1.r + sol[1]*color2.r + sol[2]*color3.r);
                int g=floor(sol[0]*color1.g + sol[1]*color2.g + sol[2]*color3.g);
                int b=floor(sol[0]*color1.b + sol[1]*color2.b + sol[2]*color3.b);
                TGAColor col = TGAColor(r, g, b, 255);
                set_color(i, j, image, col);
            }
        }
    }
}

void draw_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    // 'transponiraj' duzinu ako je veci od 1
    bool invert = false;
    if (abs(x1 - x0) < abs(y1 - y0))
    {
        swap(x0, y0);
        swap(x1, y1);
        invert = true;
    }
    
    // zamijeni tocke ako pravac ide s desna na lijevo
    if (x1 < x0)
    {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    // nacrtaj duzinu
    line_midpoint(x0, y0, x1, y1, image, color, invert);
}

int main()
{
    // definiraj sliku
    TGAImage image(width, height, TGAImage::RGB);

    vector<int> p1;
    p1.push_back(25);
    p1.push_back(40);
    vector<int> p2;
    p2.push_back(100);
    p2.push_back(100);
    vector<int> p3;
    p3.push_back(11);
    p3.push_back(210);

    vector<int> q1;
    q1.push_back(400);
    q1.push_back(350);
    vector<int> q2;
    q2.push_back(500);
    q2.push_back(420);
    vector<int> q3;
    q3.push_back(428);
    q3.push_back(500);

    vector<int> r1;
    r1.push_back(200);
    r1.push_back(200);
    vector<int> r2;
    r2.push_back(300);
    r2.push_back(350);
    vector<int> r3;
    r3.push_back(150);
    r3.push_back(400);


    draw_triangle_2d(p1, p2, p3, image, green);

    draw_triangle_2d(q1, q2, q3, image, purple);

    draw_triangle_2d_gouraud(r1, r2, r3, image, orange, cyan, yellow);

    // spremi sliku
    image.flip_vertically();
    image.write_tga_file("z1z2.tga");
}