#include <stdio.h>
#include<omp.h>

int ctruncate(float x);
int main(void) 
{ 
    int len;
    int change,val;
	double t5,t6,t7,t8;
	printf("Enter image size\n");
	scanf("%d",&len);
	int arr[len][len],i,j;
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			arr[i][j]=((i*j+i+j)%255);
		}
	}
	/*printf("Input array\n");
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d\t",arr[i][j]);
		}
		printf("\n");
	}*/
	printf("Enter \n1:increase change\n2:decrease change\n");
	scanf("%d",&change);
	printf("Enter \n1:low change\n2:medium change\n3:high change\n");
	scanf("%d",&val);
	float c;
	int d[len][len],e[len][len];
	int z = (change==1)?1:-1;
	int a = (val==1)?20:(val==2)?40:80;
	int contrast = a * z;
	float fact = (259 * (contrast + 255));
	fact /=  (255 * (259 - contrast));
	//printf("%d %d %f ",z,contrast,fact);
	t5=omp_get_wtime();
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
		    c = ((arr[i][j]-128)*fact)+128;
			d[i][j] = ctruncate(c);
		}
	}
	t6=omp_get_wtime();
	/*printf("Array after contrast change\n");
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d\t",d[i][j]);
		}
		printf("\n");
	}*/
	t7=omp_get_wtime();
	#pragma omp for collapse(2) private(i,j)
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
		    c = ((arr[i][j]-128)*fact)+128;
			e[i][j] = ctruncate(c);
		}
	}
	t8=omp_get_wtime();
	/*printf("Array after contrast change\n");
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d\t",e[i][j]);
		}
		printf("\n");
	}*/
	printf("Time for execution for contrast change in sequential: %lf\n",t6-t5);
	printf("Time for execution for contrast change in parallel: %lf\n",t8-t7);
}	
int ctruncate(float x){
    int y=(int)x;
	if(y<-255) return -255;
	if(y>255) return 255;
	return y;
}
