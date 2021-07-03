#include<stdio.h>
#include<stdlib.h>
#include "omp.h"


 
 
void convolution(int** src, int** dst, int** Kernel, int rows,int cols, int krows,int kcols)
{
    int kCenterX = kcols / 2;
	int kCenterY = krows / 2;
    int sum, x1, y1;
    int x,y,k,j,ii,jj,mm,nn;

    for(int y = 0; y < rows; y++){
        for(int x = 0; x < cols; x++){
            for(int k = 0;k < krows; k++){
                for(int j = 0;j < kcols; j++ ){
                    int mm = krows - 1 - k;     
	                int nn = kcols - 1 - j;  

	                
	                int ii = y + (kCenterY - mm);
	                int jj = x + (kCenterX - nn);
	                
	                if( ii >= 0 && ii < rows && jj >= 0 && jj < cols )
	                    dst[y][x] += src[ii][jj] * Kernel[k][j];
                }
            }
        }
    }
}
 
int main()
{
    int kCols=3;
	int kRows=3;
	int rows=10;
	int cols=10;
	
    int** in = (int**) malloc(rows*sizeof(int*));
    int** out = (int**) malloc(rows*sizeof(int*));
    int** kernel = (int**) malloc(kRows*sizeof(int*));
    for(int i=0;i<rows;i++){
        in[i]=(int*)malloc(cols*sizeof(int));
        out[i]=(int*)malloc(cols*sizeof(int));
    }
    for(int i=0;i<kRows;i++){
        kernel[i]=(int*)malloc(kCols*sizeof(int));
    }
	
    
    for(int i=0;i<kRows;i++){
		for(int j=0;j<kCols;j++){
            kernel[i][j]=i*j+i+j;
		}
	}
	for(int i=0;i<rows;i++){
		for(int j=0;j<cols;j++){
			in[i][j]=i*j+i+j;
            out[i][j]=0;
		}
	}
    double start = omp_get_wtime();
    convolution(in, out, kernel,rows,cols,kRows,kCols);
    printf("time   = %.6f secs\n", omp_get_wtime() - start);

    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            printf("\t %d",out[i][j]);
        }
        printf("\n");
    }

    
}