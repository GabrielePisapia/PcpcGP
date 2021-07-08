#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc,char *argv[]){
    
    int rank,tag=0,dest,source;
    char stringa[100];
    

    MPI_Status status;
    MPI_Init(&argc,&argv);
    int world_size,chunk,end,i,start =0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    
    int array[10] = {1,2,3,4,5,6,7,8,9,10};

    if (rank ==0){
        
        int i =0,k= 1;
        int arraylen= sizeof(array)/sizeof(int);
        chunk = arraylen/world_size;
        int remainder = arraylen - chunk*world_size;
    
        for (k;k<world_size;k++){
         /*invia start di inizio*/
            printf("Valore di k: %d \n",k);
            MPI_Send(&chunk,1,MPI_INT,k,0,MPI_COMM_WORLD);

        }
         
    }
    else{

     
       MPI_Recv(&chunk,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        int start = (rank-1)*chunk;
       for (int index = (rank-1)*chunk;index<start+chunk;index++){
           printf("Il processo %d ha ricevuto il valore: %d \n",rank,array[index]);
       }
    }
    MPI_Finalize();
    return 0;
}


// punto di partenza di lettuar = Rank -1 * chunk  ( poi leggi per chunk caselle l'array)