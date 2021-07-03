#include<bits/stdc++.h>
#include "Ivp.h"
#include<opencv4/opencv2/opencv.hpp>
using namespace std;
int main(){
    int k;
    string name = "img2.jpg";
    struct img img1 = IVP::create_img(name);
    cout<<img1.h<<" "<<img1.w<<endl;
    IVP::print(img1);
    cout<<"enter the gausian kernal size"<<endl;
    cin>>k;
    cout<<"gausian filter"<<endl;
    struct img filter = IVP::create_filter(k);
    cout<<"Blurr Operation"<<endl;
    struct img bimg = IVP::blur(img1,filter);
    IVP::print(bimg);
    cout<<"rotated"<<endl;
    struct img rotated = IVP::rotate(img1);
    IVP::print(rotated);
    struct img3d img2 = IVP::create_rgb_img(name);
    cout<<"RGB"<<endl;
    IVP::print3d(img2);
    struct img3d img2yuv = IVP::rgb2yuv(img2);
    cout<<"rgb2yuv"<<endl;
    IVP::print3d(img2yuv);
    img2yuv = IVP::yuv2rgb(img2yuv);
    cout<<"yuv2rgb"<<endl;
    IVP::print3d(img2yuv);
    struct img gray = IVP::rgb2gray(img2);
    cout<<"gray"<<endl;
    IVP::print(gray);
    IVP::contrast_(img1);
    IVP::brightness(img1);
}