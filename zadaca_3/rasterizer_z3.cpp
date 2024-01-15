#include <iostream>
#include <cmath>
#include <vector>
#include "tgaimage.h"
using namespace std;

// g++ rasterizer_z3.cpp tgaimage.cpp -o out1.exe

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

void intify(vector<float> x, vector<int>& xi){
    xi.clear();
    xi.push_back((int)x[0]);
    xi.push_back((int)x[1]);
}

void translate(vector<float>& x, vector<float> t, bool neg=false){
    if(neg==true){
        t[0]=-t[0];
        t[1]=-t[1];
    }
    for(int i=0; i<2; i++){
        x[i]-=t[i];
    }
}

void scale(vector<float>& x, float s){
    for(int i=0; i<2; i++){
        x[i]*=s;
    }
}

void perspective_transform(vector<int> x, vector<float>& x1){
    x1.clear();
    x1.push_back((float)x[0]/(float)x[2]);
    x1.push_back((float)x[1]/(float)x[2]);
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

void draw_triangle(vector<int> x1, vector<int> y1, vector<int> z1, TGAImage &image, TGAColor color){
    vector<float> x;
    vector<float> y;
    vector<float> z;
    vector<int> xi;
    vector<int> yi;
    vector<int> zi;
    vector<float> t(2);
    perspective_transform(x1, x);
    t=x;
    translate(x, t);
    scale(x, (float)x1[2]);
    translate(x, t, true);
    perspective_transform(y1, y);
    translate(y, t);
    scale(y, (float)y1[2]);
    translate(y, t, true);
    perspective_transform(z1, z);
    translate(z, t);
    scale(z, (float)z1[2]);
    translate(z, t, true);
    intify(x, xi);
    intify(y, yi);
    intify(z, zi);
    xi[0]+=256;
    xi[1]+=256;
    yi[0]+=256;
    yi[1]+=256;
    zi[0]+=256;
    zi[1]+=256;
    draw_triangle_2d(xi, yi, zi, image, color);
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
    p1.push_back(-48);
    p1.push_back(-10);
    p1.push_back(82);
    vector<int> p2;
    p2.push_back(29);
    p2.push_back(-15);
    p2.push_back(44);
    vector<int> p3;
    p3.push_back(13);
    p3.push_back(34);
    p3.push_back(114);

    draw_triangle(p1, p2, p3, image, blue);

    // spremi sliku
    image.flip_vertically();
    image.write_tga_file("z3.tga");
}
