#include "mpi.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    int numtasks, rank, count;
    int dest, source;
    MPI_Status Stat;
    int inmsg, outmsg = 7;

    //Inizializzazione di MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        dest = 1;
        MPI_Send(&outmsg, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        printf("Process with rank: %d send: %d. \n", rank, outmsg);
    }
    else{
        source = 0;
        MPI_Recv(&inmsg, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &Stat);
        printf("Process with rank: %d receive: %d. \n", rank, inmsg);
    }
    MPI_Finalize();
}