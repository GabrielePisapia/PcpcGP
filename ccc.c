#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "mpi.h"


int main (int argc,char *argv[]){

    int rank,tag,dest,source,world_size;
    int n;
    int a[n];

    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (rank != 0){
        n= 10;
        printf("my n: %d",n);
        
    }
        
    if (rank ==0 ){
       n = 6;
       printf("my n: %d",n);
    }
    MPI_Finalize();
    return 0;
    

}