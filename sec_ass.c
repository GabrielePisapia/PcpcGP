#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc,char *argv[]){
    
    int rank,tag,dest,source;
    char stringa[100];
    

    MPI_Status status;
    MPI_Init(&argc,&argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    
    int array[10] = {1,2,3,4,5,6,7,8,9,10};

    if (rank ==0){
        printf("Inserisci la stringa da inviare: ");
        fflush(stdout);
        scanf("%s",stringa);
        printf("\n");
        MPI_Send(&stringa,strlen(stringa)+1,MPI_CHAR,1,99,MPI_COMM_WORLD);
        printf("Sto inviando la stringa: %s \n", stringa);
    }
    else if (rank==1){
        MPI_Recv(&stringa,100,MPI_CHAR,0,99,MPI_COMM_WORLD,&status);
        printf("Recived: %s",stringa);
    }
    MPI_Finalize();
    return 0;
}