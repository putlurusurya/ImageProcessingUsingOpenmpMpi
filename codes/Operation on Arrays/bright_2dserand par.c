#include <stdio.h>
#include<omp.h>

int truncate(int x);
int main(void) 
{ 
    int len;
    int change,val;
	double t1,t2,t3,t4;
	printf("Enter image size\n");
	scanf("%d",&len);
	int arr[len][len],b[len][len],i,j;
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			arr[i][j]=(i*j+i+j)%255;
		}
	}
	/*printf("Input array\n");
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d ",arr[i][j]);
		}
		printf("\n");
	}*/
	printf("Enter \n1:increase change\n2:decrease change\n");
	scanf("%d",&change);
	printf("Enter \n1:low change\n2:medium change\n3:high change\n");
	scanf("%d",&val);
	int z = (change==1)?1:-1;
	int factor = (val==1)?20:(val==2)?40:80;
	int brichange = factor * z;
	t1=omp_get_wtime(); 
	for(i=0;i<len;i++){
		for(j=0;j<len;j++){
			b[i][j]=truncate(arr[i][j]+brichange);
		}
	}
	t2=omp_get_wtime();
	/*printf("Array after brightness change\n");
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d\t",b[i][j]);
		}
		printf("\n");
	}*/
	t3=omp_get_wtime();
	#pragma omp for collapse(2) private(i,j) 
	for(i=0;i<len;i++){
		for(j=0;j<len;j++){
			b[i][j]=truncate(arr[i][j]+brichange);
		}
	}
	t4=omp_get_wtime();
	/*printf("Array after brightness change\n");
	for(int i=0;i<len;i++){
		for(int j=0;j<len;j++){
			printf("%d\t",b[i][j]);
		}
		printf("\n");
	}*/
	printf("Time for execution for brightness change in sequential: %lf\n",t2-t1);
	printf("Time for execution for brightness change in parallel: %lf\n",t4-t3);
}
int truncate(int x){
	if(x<0) return 0;
	if(x>255) return 255;
	return x;
}