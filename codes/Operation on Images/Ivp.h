#ifndef ivp_h
#define ivp_h
#include<bits/stdc++.h>
struct img {
    int h,w;
    float** arr;
};
struct img3d {
    int h,w,c;
    float*** arr;
};
namespace IVP{
        struct img create_img(std::string path);
        struct img3d rgb2yuv(struct img3d);
        struct img3d yuv2rgb(struct img3d);
        struct img rgb2gray(struct img3d);
        struct img3d create_rgb_img(std::string);
        struct img create_filter(int);
        struct img blur(struct img,struct img);
        struct img rotate(struct img);
        struct img brightness(struct img);
        struct img contrast_(struct img);
        void print(struct img);
        void print3d(struct img3d);
};

#endif
