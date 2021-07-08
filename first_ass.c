#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc,char *argv[]){
    
    int rank,tag,dest,source;
    int toSend = 100;

    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);


    if (rank ==0){
        MPI_Send(&toSend,1,MPI_INT,1,99,MPI_COMM_WORLD);
        printf("Sto inviando il valore: %d", toSend);
    }
    else if (rank==1){
        MPI_Recv(&toSend,1,MPI_INT,0,99,MPI_COMM_WORLD,&status);
        printf("Recived: %d, with status",toSend);
    }
    MPI_Finalize();
    return 0;
}