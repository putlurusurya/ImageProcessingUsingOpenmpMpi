#include <stdio.h>
#include<stdlib.h>
#include "omp.h"
#include "mpi.h"


int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}

void convoluteGray(int** src, int** dst, int krows, int kcols, int** h,int rows,int cols) {
	int x,y,k,j,p,q;
    int kCenterX = kcols / 2;
	int kCenterY = krows / 2;

    #pragma omp parallel for num_threads(8) shared(src, dst,h) schedule(static) collapse(4)
    for(y = 0; y <= rows-1; y++){
        for( x = 0; x <= cols-1; x++){
            for( k=0;k<krows;k++){
                for( j=0;j<kcols;j++){
                    //printf("y= %d,x= %d,k: %d,j: %d,l= %d, m= %d\n",y,x,k,j,l,m);
                    q = y-kCenterX ;
                    p = x-kCenterY ;
                    if(q+k>=0 && q+k<rows && p+j>=0 && p+j<cols){
                        dst[y][x]+=src[q+k][p+j]*h[k][j];
                    }
                    //printf("y= %d,x= %d,k: %d,j: %d,q= %d, p= %d, dst[y][x]= %d\n",y,x,k,j,q,p,dst[y][x]);
                }
            }
        }
    }
}
	
void convolutePartial(int** src, int** dst, int krows, int kcols, int** h,int rows,int cols) {
	int x,y,k,j,p,q;
    int kCenterX = kcols / 2;
	int kCenterY = krows / 2;

    #pragma omp parallel for num_threads(8) shared(src, dst) schedule(static) collapse(4)
    for(y = kCenterX; y < rows-kCenterX; y++){
        for( x = 0; x <= cols-1; x++){
            for( k=0;k<krows;k++){
                for( j=0;j<kcols;j++){
                    //printf("y= %d,x= %d,k: %d,j: %d,l= %d, m= %d\n",y,x,k,j,l,m);
                    q = y-kCenterX ;
                    p = x-kCenterY ;
                    if(q+k>=0 && q+k<rows && p+j>=0 && p+j<cols){
                        dst[y-kCenterX][x]+=src[q+k][p+j]*h[k][j];
                    }
                    //printf("y= %d,x= %d,k: %d,j: %d,q= %d, p= %d, dst[y][x]= %d\n",y,x,k,j,q,p,dst[y][x]);
                }
            }
        }
    }
	
}

int** padding(int** src,int rows,int cols,int rowadd){
    int** out = alloc_2d_int(rows+2*rowadd,cols);
    for(int i=0;i<rows+2*rowadd;i++){
        for(int j=0;j<cols;j++){
            if(i>=rowadd&&i<rows+rowadd){
                out[i][j]=src[i-rowadd][j];
            }
            else{
                out[i][j]=0;
            }
        }
    }
    return out;
}
/*
int divide_rows(int rows, int cols, int workers) {
    int per, rows_to, cols_to, best = 0;
    int per_min = rows + cols + 1;
    for (rows_to = 1 ; rows_to <= workers ; ++rows_to) {
        if (workers % rows_to || rows % rows_to) continue;
        cols_to = workers / rows_to;
        if (cols % cols_to) continue;
        per = rows / rows_to + cols / cols_to;
        if (per < per_min) {
            per_min = per;
            best = rows_to;
        }
    }
    return best;
}
*/

int main(int argc,char *argv[])
{
    int kCols=3;
	int kRows=3;
	int rows=500;
	int cols=500;
    int krowsh=kRows/2;
    int kcolsh=kCols/2;

    int process_id, num_processes;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    
    int** in = alloc_2d_int(rows,cols);
    int** out = alloc_2d_int(rows,cols);
    int** kernel = alloc_2d_int(kRows,kCols);

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
    //convolution(in, out, kernel,rows,cols,kRows,kCols);
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&kRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&kCols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&(kernel[0][0]), kRows*kCols, MPI_INT, 0, MPI_COMM_WORLD);

    
    
    if(num_processes==1||num_processes>rows){
        convoluteGray(in, out, kRows, kCols, kernel,rows,cols);
    }
    else{
        int prows[num_processes];
        int firstrow[num_processes];
        int splitr;
        int extrar;

        splitr = rows / num_processes;
        extrar = rows % num_processes;
        
        for (int i = 0; i < extrar; ++i) 
            prows[i] = (splitr + 1)*cols;
        for (int i = extrar; i < num_processes; ++i) 
            prows[i] = (splitr)*cols;
        firstrow[0]=krowsh*cols;
        for (int i = 1; i < num_processes; ++i) 
            firstrow[i] = firstrow[i-1] + prows[i-1];
        
        for(int i=0;i<num_processes;i++){
            prows[i] += krowsh*2*cols;
                firstrow[i] -= krowsh*cols;
        }
    
        int** paddedin=padding(in,rows,cols,krowsh);
        int localr=prows[process_id];
        int** local_mat= alloc_2d_int(localr/cols,cols);
        int** local_output= alloc_2d_int(localr/cols-2*krowsh,cols);
        
        MPI_Scatterv(&(paddedin[0][0]), &prows[0], &firstrow[0] ,MPI_INT, &(local_mat[0][0]), localr, MPI_INT, 0, MPI_COMM_WORLD);
        convolutePartial(local_mat,local_output,kRows,kCols,kernel,localr/cols,cols);
        MPI_Gatherv(&local_output[0][0], prows[process_id]-cols*2*krowsh, MPI_INT,&(out[0][0]), &prows[0], &firstrow[0], MPI_INT,0, MPI_COMM_WORLD);
    }
    
    //convoluteGray(in, out, 5*process_id,5*(process_id+1)-1, 0, cols-1, kRows, kCols, kernel,rows,cols);
    
   /* if(process_id==1){
    for(int i=0;i<localr/cols-2;i++){
        for(int j=0;j<cols;j++){
            printf("\t %d", local_output[i][j]);
        }
        printf("\n");
    }
    }*/
    //int t=divide_rows(rows, cols, 32);
    //printf("t: %d\n",t);
/*
    if(process_id==0){
    
    for(int i=0;i<rows;i++){
        for(int j=0;j<cols;j++){
            printf("\t %d",out[i][j]);
            //printf("\t %d",in[i][j]);
        }
        printf("\n");
    }
    }
    */

/*
    for(int i=0;i<num_processes;i++){
        printf("p= %d, f= %d\n",prows[i],firstrow[i]);
    }*/
    if(process_id==0){
        printf("time   = %.6f secs\n", omp_get_wtime() - start);
    }

    MPI_Finalize();
    
    //
    return 0;

    
}