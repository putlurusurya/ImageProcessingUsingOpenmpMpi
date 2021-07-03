#include "Ivp.h"
#include<bits/stdc++.h>
#include<opencv4/opencv2/opencv.hpp>
#include<omp.h>
using namespace std;
using namespace cv;
const int no_threads = 8;
const int chunk = 3;
// loding image from path and converting it into an array
struct img IVP::create_img(std::string path){
    struct img img1;
    Mat image = imread(path,IMREAD_GRAYSCALE);
    cout<<image.isContinuous()<<endl;
    cout<<image.type()<<endl;
    img1.h=image.cols;
    img1.w=image.rows;
    // dynamically allocating the array
    img1.arr = new float*[img1.h];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < img1.h; i++){
       img1.arr[i] = new float[img1.w];
    }
	int i,j;
    // assigning the random values in range 0-255
    for(i=0;i<img1.w;i++){
        for(j=0;j<img1.h;j++)
        img1.arr[j][i]=image.at<uint8_t>(i,j);
    }
	t2=omp_get_wtime();
	printf("Time for execution for creating 2D image in serial: %lf\n",t2-t1);
    return img1;
}
// converting back into mat and displaying the result
void IVP::print(struct img img1){
    Mat image(img1.w,img1.h,CV_8U);
	int i,j;
	double t1,t2;
	t1=omp_get_wtime();
    for(i=0;i<img1.w;i++){
        for (j = 0; j < img1.h; j++){
            image.at<uint8_t>(i,j)=img1.arr[j][i];
        }
    }
	t2=omp_get_wtime();
	// printf("Time for execution for printing 2D image in serial: %lf\n",t2-t1);
    std::string name ="opencv in c++";
    cv::namedWindow(name); // Create a window
    cv::imshow(name, image);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(name);
}
struct img IVP::create_filter(int n){
    assert(n%2==1);
    struct img f;
    int i,j,x,y;
    f.arr = new float*[n];
	double t1,t2;
	t1=omp_get_wtime();
    for (int i = 0; i < n; i++){
       f.arr[i] = new float[n];
    }
    // intialising standard deviation to 1.0
    float sigma = 1.0;
    float r, s = 2.0 * sigma * sigma;
  
    // sum is for normalization
    float sum = 0.0;
  
    // generating nxn kernel
    for (x = -n/2; x <= n/2; x++) {
        for (y = -n/2; y <= n/2; y++) {
            r = sqrt(x * x + y * y);
            f.arr[x + n/2][y + n/2] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += f.arr[x + n/2][y + n/2];
        }
    }

  
    // normalising the Kernel
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            f.arr[i][j] /= sum;
    f.h=n;
    f.w=n;
    t2=omp_get_wtime();
    printf("Time for execution for creating filter in serial: %lf\n",t2-t1);
    return f;
}

struct img IVP::blur(struct img img1,struct img f){
    assert(f.h<img1.h and f.w<img1.w);
    int i,j,x,y;
    // convolution
    int kCols=f.w;
	int kRows=f.h;
	int rows=img1.h;
	int cols=img1.w;
	int outSize1=rows-kRows+1;
	int outSize2=cols-kCols+1;
    struct img out;
    out.h=outSize1;
    out.w=outSize2;
    out.arr = new float*[outSize1];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < outSize1; i++){
       out.arr[i] = new float[outSize2];
    }

    for(x=0;x<outSize1;x++){
        for (y=0;y<outSize2;y++){
            out.arr[x][y]=0;
        }
    }

	int m,n,ii=0,jj=0;
	for(i=0; i < rows; ++i)              
	{

		for(j=0; j < cols; ++j)          
		{
			int l = min(i+kRows,rows),k=min(j+kCols,cols);
			double sum=0;
            for(m=i; m < l;m++)     
            {      

                for(n=j; n <k;n++) 
                {   
                    sum+=img1.arr[m][n]*f.arr[m-i][n-j];
                }
            }
			if(i<=rows-kRows and j<=cols-kCols){
				out.arr[ii][jj]+=sum;
				jj++;
			}
		}
		if(i<=rows-kRows)ii++;
		jj=0;
	}
	t2=omp_get_wtime();
	printf("Time for execution for blurring of image in serial: %lf\n",t2-t1);
    return out;
}

// performs rotation by only 90deg
struct img IVP::rotate(struct img img1){
    int n =img1.h;
    int m = img1.w;
    struct img out;
    int i,j;
    out.arr = new float*[m];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < m; i++){
       out.arr[i] = new float[n];
    }
    out.h=img1.w;
    out.w=img1.h;
    for (i=0; i<n; i++)
        for (j=0;j<m; j++)
            out.arr[j][n-1-i] = img1.arr[i][j];
	t2=omp_get_wtime();
	printf("Time for execution for rotation of image in serial: %lf\n",t2-t1);
    return out;
}

struct img3d IVP::create_rgb_img(string path){
    struct img3d img1;
    Mat image = imread(path);
    img1.h=image.cols;
    img1.w=image.rows;
    img1.c=image.channels();
    img1.arr = new float**[img1.h];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < img1.h; i++){
       img1.arr[i] = new float*[img1.w];
       for(int j=0;j<img1.w;j++)img1.arr[i][j]=new float[img1.c];
    }
	int i,j,k;

    for(i=0;i<img1.w;i++){
        for(j=0;j<img1.h;j++){
            for(k=0;k<img1.c;k++){
                img1.arr[j][i][k]=image.at<uint16_t>(i,j,k);
            }
        }
    }

	t2=omp_get_wtime();
	printf("Time for execution for creating 3d image in serial: %lf\n",t2-t1);
    return img1;
}

void IVP::print3d(struct img3d img1){
    int h=img1.h;
    int w = img1.w;
    int c = img1.c;
    int i,j;
    Mat image1(w,h,CV_16U),image2(w,h,CV_16U),image3(w,h,CV_16U);
	double t1,t2;
    t1=omp_get_wtime();
    for(int i=0;i<w;i++){
        for(int j=0;j<h;j++){
            image1.at<uint16_t>(i,j)=img1.arr[j][i][0];
            image2.at<uint16_t>(i,j)=img1.arr[j][i][1];
            image3.at<uint16_t>(i,j)=img1.arr[j][i][2];
        }
    }
	t2=omp_get_wtime();
	printf("Time for execution for printing 3d image in serial: %lf\n",t2-t1);
    Mat finalimg;
    vector<Mat> channels(3);
    channels[0]=image2;
    channels[1]=image1;
    channels[2]=image3;
    merge(channels,finalimg);
    std::string name ="opencv in c++";
    cv::namedWindow(name); // Create a window
    cv::imshow(name, finalimg);
    cv::waitKey(0); // Wait for any keystroke in the window
    cv::destroyWindow(name);
}
struct img3d IVP::rgb2yuv(struct img3d img1){
    struct img3d yuv;
    int i,j;
    yuv.arr = new float**[img1.h];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < img1.h; i++){
       yuv.arr[i] = new float*[img1.w];
       for(int j=0;j<img1.w;j++)yuv.arr[i][j]=new float[img1.c];
    }
    yuv.h=img1.h;
    yuv.w=img1.w;
    yuv.c=img1.c;

    for(i=0;i<img1.h;i++){
        for(j=0;j<img1.w;j++){
            yuv.arr[i][j][0]=(int(66*img1.arr[i][j][0]+ 129 *img1.arr[i][j][1] +  25 *img1.arr[i][j][2]+128)>>8)+16;
            yuv.arr[i][j][1]=(int(-38*img1.arr[i][j][0]-74*img1.arr[i][j][1] + 112*img1.arr[i][j][2]+128)>>8)+128;
            yuv.arr[i][j][2]=(int(112 *img1.arr[i][j][0] -  94*img1.arr[i][j][1] -  18*img1.arr[i][j][2] + 128) >> 8) + 128;
        }
    }
	t2=omp_get_wtime();
    return yuv;
    printf("Time for execution for RGB to YUV image change in serial: %lf\n",t2-t1);
}

struct img3d IVP::yuv2rgb(struct img3d img1){
    struct img3d rgb;
    int n=img1.h;
    int i,j;
    rgb.arr = new float**[n];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < n; i++){
       rgb.arr[i] = new float*[img1.w];
       for(int j=0;j<img1.w;j++)rgb.arr[i][j]=new float[img1.c];
    }
    rgb.h=img1.h;
    rgb.w=img1.w;
    rgb.c=img1.c;

    for(i=0;i<n;i++){
        for(j=0;j<img1.w;j++){
            rgb.arr[i][j][0]=int(298*(img1.arr[i][j][0]-16)+409*(img1.arr[i][j][2]-128)+128)>>8;
            rgb.arr[i][j][1]=int(298*(img1.arr[i][j][0]-16)-100*(img1.arr[i][j][1]-128)-208*(img1.arr[i][j][2]-128)+128)>>8;
            rgb.arr[i][j][2]=int(298*(img1.arr[i][j][0]-16)+516*(img1.arr[i][j][1]-128)+128)>>8;
        }
    }
	t2=omp_get_wtime();
	printf("Time for execution for YUV to RGB image change in serial: %lf\n",t2-t1);
    return rgb;
}

struct img IVP::rgb2gray(struct img3d img1){
    struct img gray;
    int i,j,k;
    // dynamically allocating the array
    gray.h=img1.h;
    gray.w = img1.w;
    gray.arr = new float*[gray.h];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < gray.h; i++){
       gray.arr[i] = new float[gray.w];
    }
    //2 0 1 b g r
    float max_v=0;
    for(i=0;i<img1.h;i++){
        for(j=0;j<img1.w;j++){
            float val=0;
            for(k=0;k<img1.c;k++){
                val+=img1.arr[i][j][k];
            }
            max_v = std::max(max_v,val);
            // cout<<val<<endl;
            gray.arr[i][j]=val;
        }
    }

    for(i=0;i<img1.h;i++){
        for(j=0;j<img1.w;j++){
            gray.arr[i][j]/=max_v;
            gray.arr[i][j]*=255;
        }
    }
	t2=omp_get_wtime();
	printf("Time for execution for RGB to grayscale image change in serial: %lf\n",t2-t1);
    return gray;
}
// phanis
int truncate(int x){
	if(x<0) return 0;
	if(x>255) return 255;
	return x;
}
struct img IVP::brightness(struct img img1){
    IVP::print(img1);
    int change,val;
    printf("Enter \n1:increase change\n2:decrease change\n");
	scanf("%d",&change);
	printf("Enter \n1:low change\n2:medium change\n3:high change\n");
	scanf("%d",&val);
	int z = (change==1)?1:-1;
	int factor = (val==1)?20:(val==2)?40:80;
	int brichange = factor * z;
    int h = img1.h;
    int w = img1.w;
    struct img out;
    // dynamically allocating the array
    out.h=img1.h;
    out.w = img1.w;
    out.arr = new float*[out.h];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < out.h; i++){
       out.arr[i] = new float[out.w];
    }
	int i,j;

    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            out.arr[i][j]=truncate(img1.arr[i][j]+brichange)%255;
        }
    }
	t2=omp_get_wtime();
	printf("Time for execution for brightness change in serial: %lf\n",t2-t1);
    IVP::print(out);
    return out;
}
int ctruncate(float x){
    int y=(int)x;
	if(y<-255) return -255;
	if(y>255) return 255;
	return y;
}
struct img IVP::contrast_(struct img img1){
    IVP::print(img1);
    int change,val,i,j;
    printf("Enter \n1:increase change\n2:decrease change\n");
	scanf("%d",&change);
	printf("Enter \n1:low change\n2:medium change\n3:high change\n");
	scanf("%d",&val);
	float c;
	int z = (change==1)?1:-1;
	int a = (val==1)?20:(val==2)?40:80;
	int contrast = a * z;
	float fact = (259 * (contrast + 255));
	fact /=  (255 * (259 - contrast));
    int h = img1.h;
    int w = img1.w;
    struct img out;
    // dynamically allocating the array
    out.h=img1.h;
    out.w = img1.w;
    out.arr = new float*[out.h];
	double t1,t2;
    t1=omp_get_wtime();
    for (int i = 0; i < out.h; i++){
       out.arr[i] = new float[out.w];
    }

    for(i=0;i<h;i++){
        for(j=0;j<w;j++){
            c = ((img1.arr[i][j]-128)*fact)+128;
            out.arr[i][j] = ctruncate(c);
        }
    }

	t2=omp_get_wtime();
	printf("Time for execution for contrast change in serial: %lf\n",t2-t1);
    IVP::print(out);
    return out;
}