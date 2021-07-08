#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

int main (int argc,char *argv[]){
    
    /*Useful variables init*/
    int rank,tag=0,dest,source;
    int word_count = 0, in_word = 0,local_wc=0;
    int ch;
    DIR *d;
    FILE *fp;
    struct dirent *dir;
    char path[2100];
    char file_name_and_number_words [2][50];
    int how_much_word [] ={0,0};
   
    /*Mpi init*/

    MPI_Status status;
    MPI_Init(&argc,&argv);
    int world_size,chunk,end,i,start =0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    

    if (rank ==0){
        int while_counter = 0;
        d=opendir("myfolder");
            if(d){
                while((dir = readdir(d)) != NULL){  // occhio agli array che legge due file a caso: .. .
                    if(strcmp(dir->d_name,".") !=0 && strcmp(dir->d_name,"..") !=0){
                    strcpy(path,"myfolder");
                    strcat(path,"/");
                    strcat(path,dir->d_name);
                    strcpy(file_name_and_number_words[while_counter],dir->d_name);
                    fp = fopen(path,"r");

                    if(fp == NULL){
                        perror("File non aperto, c'è stato un problema");
                    }

                    while ((ch = fgetc(fp)) != EOF){
                        if(isalnum(ch)!=0 ){
                            in_word =1;

                        }/*fine if carattere alfanumerico*/
                        else{
                            if ((ch == ' ' || ch == '\t' || ch== '\n') && (in_word==1)){
                                local_wc++;
                                
                                word_count++;
                                in_word=0;
                            }/*fine if tabulazione*/
                        }/*fine else non è un carattere*/
                    }/*fine del while per lettura caratteri dal file*/
                    printf("\n");
                    how_much_word[while_counter] = local_wc;
                    //printf("The file: %s has got %d words \n", dir->d_name,local_wc);
                    while_counter++;
                    local_wc=0;
                    }
                }/*fine while lettura della dir*/

            }/*end if opendir*/
        fclose(fp);
        closedir(d);

        printf("Total number of words in all files is: %d \n",word_count);

        for(int in =0; in<2;in++){
                printf("The file %s has got %d words \n", file_name_and_number_words[in],how_much_word[in]);
            }



        int partition = word_count/world_size;
        int local_partition = partition;
        int remainder = word_count - (partition * world_size);
        int lowerbound =0 ,upperbound =0;

       // Invia la partition e il resto a tutti gli altri processi
       // Invia l'array contenente i nomi dei file e l'altro array delle quantità

       /*Calcolo lavoro per il processo master*/
       
       if (remainder != 0){  // La divisione non è equa, di conseguenza il processo master deve prendere sicuramente un elemento in più, altrimenti sarebbe a resto 0
            lowerbound = 0; /*Da che riga deve iniziare a leggere*/
            upperbound = (partition * (rank+1)) +1;
       }else{ // divisione equa
            lowerbound = 0;
            upperbound = partiton * (rank +1);
       }


       int ind = 0;
       /* Inizio del lavoro del master */
       while (local_partition  <= 0){
           /* controlliamo quanti file leggere, ci possono essere 3 casi:
              1) il file è più piccolo rispetto alla partition
              2) il file è più grande rispetto alla partition
              3) il file è esattamente grande quanto la partition */

           // caso (1)
           if (how_much_word[ind] < partition){
               if(local_partition>how_much_word[ind]){
                   // leggi tutto
                   local_partition -= how_much_word[ind];
               } else{
                   // leggi fino a local partition
                   local_partition -= how_much_word[ind];
               }
               
           }

           // caso (2)
           if(how_much_word[ind]>partition){
               // leggi il file da linea lowerbound fino ad upperbound
               local_partition =0;
           }

           // caso (3)
           if(how_much_word[ind]==partition){
               // leggi il file per intero. Non c'è bisogno di contare le linee
               local_partition = 0;
           }
           ind++;
       }
    }/* endif rank=0*/
    
    else{   /*slaves*/
            // recive resto e partition
            // recive dei due array quello dei nomi dei file e quello del contatore delle parole
            
            int lowerbound = 0, upperbound = 0;

            /*Start algoritmo di calcolo della quantità da leggere*/
            if ( remainder !=0 ){

                if (rank<remainder){
                    lowerbound = (partition + 1) * rank;
                    upperbound = ((partition+1)* rank)+1;
                }else{
                    lowerbound = (partition*rank)+remainder;
                    upperbound = (partition * (rank+1))+remainder;
                }
            } else { /* non c'è resto*/
                lowerbound = partition*rank;
                upperbound = partition * (rank+1);
            }

            /* Quali file deve leggere? */

             // scandisci l'array di numero di parole
             // ad ogni passo se il numero di parole è minore di lower bound, passa al file successivo, sommando il numero di parole progressivamente
             // quando si arriva alla ocndizione in cui il numero di parole cumulato è maggiore di lower bound, significa che abbiamo trovato il file da cui inizieremo a leggere
             //
    }

        
        
         
    
    MPI_Finalize();
    return 0;
}


