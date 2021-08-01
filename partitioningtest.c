#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

/* Data struct for mantaining list of words and their counts */
struct WordCounter
{
  char *word;
  int word_count;
  struct WordCounter *pNext;                        /* Pointer to the next word counter in the list */
};

/* Function prototypes */
void addWord(char *pWord);                          /* Adds a word to the list or updates exisiting word */
int is_separator(char ch);                          /* Tests for a separator character */
void show(struct WordCounter *pWordcounter,int rank);        /* Outputs a word and its count of occurrences */
struct WordCounter* createWordCounter(char *word);  /* Creates a new WordCounter structure */
int number_non_duplicate_words();                    /*Count how much word there are in the struct (non duplicate words)*/

/* Global variables */
struct WordCounter *pStart = NULL;                 /* Pointer to first word counter in the list */

/* Implementation of functions for the data struct */

void show(struct WordCounter *pWordcounter,int rank)
{
  /* output the word left-justified in a fixed field width followed by the count */
  printf("\n [RANK %d]: %s   %d",rank, pWordcounter->word,pWordcounter->word_count);
}

int number_non_duplicate_words(){ 

    struct WordCounter *pCounter = NULL;
    struct WordCounter *pLast = NULL;
    pCounter = pStart;
    int returnvalue = 0;
    while(pCounter != NULL){
        returnvalue++;
        pCounter = pCounter->pNext;
    }
    return returnvalue;
}

void addWord(char *word)
{
  struct WordCounter *pCounter = NULL;
  struct WordCounter *pLast = NULL;

  if(pStart == NULL)
  {
    pStart = createWordCounter(word);
    return;
  }

  /* If the word is in the list, increment its count */
  pCounter = pStart;
  while(pCounter != NULL)
  {
    if(strcmp(word, pCounter->word) == 0)
    {
      ++pCounter->word_count;
      return;
    }
    pLast = pCounter;            /* Save address of last in case we need it */
    pCounter = pCounter->pNext;  /* Move pointer to next in the list        */
  }
 
  /* If we get to here it's not in the list - so add it */
  pLast->pNext = createWordCounter(word);
}

/* Create and returns a new WordCounter object for the argument */
struct WordCounter* createWordCounter(char *word)
{
  struct WordCounter *pCounter = NULL;
  pCounter = (struct WordCounter*)malloc(sizeof(struct WordCounter));
  pCounter->word = (char*)malloc(strlen(word)+1);
  strcpy(pCounter->word, word);
  pCounter->word_count = 1;
  pCounter->pNext = NULL;
  return pCounter;
}

int main (int argc,char *argv[]){
    
    /*Useful variables init*/
    int rank,tag=0,dest,source,partition,remainder,in_word=0,how_much_file;
    int lowerbound =0 ,upperbound =0,local_partition =0,word_count=0,local_wc=0; int readed_nd_word = 0;
    int num_car =0;
    

       

    char path[2100];
    int ch,index_of_tmpword=0;
    char tmpword[200];
    struct WordCounter *pCounter = NULL;
    char *result_word;
    char *exactly_word;

    MPI_Status status;
    MPI_Init(&argc,&argv);
    int world_size,chunk,end,i,start =0,numberfile=0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    int recv[world_size];
    int disp[world_size];
    int recvs_counts[world_size];
    

       if (rank ==0){
           
            
            DIR *d;
            FILE *fp;    
            struct dirent *dir;
          
           
            
            d=opendir("myfolder");
                if(d){
                    while((dir = readdir(d)) != NULL){  
                        if(strcmp(dir->d_name,".") !=0 && strcmp(dir->d_name,"..") !=0){
                            numberfile++;
                        }
                    }
                }
            closedir(d);
            int how_much_word[numberfile];
            char file_name_and_number_words[numberfile][100];
            int while_counter = 0;
            d=opendir("myfolder");
                if(d){
                    while((dir = readdir(d)) != NULL){  
                        if(strcmp(dir->d_name,".") !=0 && strcmp(dir->d_name,"..") !=0){
                        strcpy(path,"myfolder/");
                        strcat(path,dir->d_name);
                        strcpy(file_name_and_number_words[while_counter],dir->d_name);
                        printf("FILE: %s \n",dir->d_name);
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

        for(int in =0; in<numberfile;in++){
                printf("The file %s has got %d words \n", file_name_and_number_words[in],how_much_word[in]);
            }



        partition = word_count/world_size;
        remainder = word_count - (partition * world_size);
       
        for (int tmp = 1; tmp<world_size;tmp++){

            MPI_Send(&numberfile,1,MPI_INT,tmp,99,MPI_COMM_WORLD);
            MPI_Send(file_name_and_number_words,sizeof(file_name_and_number_words)/sizeof(file_name_and_number_words[0][0]),MPI_CHAR,tmp,0,MPI_COMM_WORLD);
            MPI_Send(how_much_word,sizeof(how_much_word)/sizeof(how_much_word[0]),MPI_INT,tmp,1,MPI_COMM_WORLD);
            MPI_Send(&partition,1,MPI_INT,tmp,2,MPI_COMM_WORLD);
            MPI_Send(&remainder,1,MPI_INT,tmp,3,MPI_COMM_WORLD);
        }
        

       // Invia la partition e il resto a tutti gli altri processi
       // Invia l'array contenente i nomi dei file e l'altro array delle quantità

       /*Calcolo lavoro per il processo master*/
       
       if (remainder != 0){  // La divisione non è equa, di conseguenza il processo master deve prendere sicuramente un elemento in più, altrimenti sarebbe a resto 0
            lowerbound = 0; /*Da che riga deve iniziare a leggere*/
            upperbound = (partition * (rank+1)) +1;
            local_partition = partition+1;
       }else{ // divisione equa
            lowerbound = 0;
            upperbound = partition * (rank +1);
            local_partition = partition;
       }


       int ind = 0;
       char path_to_read [800];
       printf("Partition = %d, Local partition = %d \n", partition, local_partition);
       

        /* Reset delle variabili già utilizzate */
        in_word=0;
        word_count = 0;

       /* Inizio del lavoro del master */
       
       while (local_partition  > 0){
           strcpy (path_to_read,"myfolder/");
           /* controlliamo quanti file leggere, ci possono essere 3 casi:
              1) il file è più piccolo rispetto alla partition
              2) il file è più grande rispetto alla partition
              3) il file è esattamente grande quanto la partition */
              strcat(path_to_read,file_name_and_number_words[ind]);
              printf("[MASTER] file e directory: %s \n",path_to_read);
            
              fp = fopen(path_to_read,"r");
              
             

              if(fp == NULL){
                  perror("[MASTER] errore nella lettura del file");
              }

              while((ch = fgetc(fp)) != EOF){

                        if(isalnum(ch)!=0 ){
                            in_word =1;
                            ch = tolower(ch);
                            tmpword[index_of_tmpword] = ch;
                            index_of_tmpword++;  

                        } else{
                            if ((ch == ' ' || ch == '\t' || ch== '\n') && (in_word==1)){
                                word_count++;
                                in_word=0;
                                local_partition--;
                                tmpword[index_of_tmpword] ='\0';
                                index_of_tmpword++;
                                addWord(tmpword);
                                memset(tmpword,0,200);
                                index_of_tmpword = 0;
                                if(local_partition<0){
                                    break;
                                }
                            }/*fine if tabulazione*/
                        } 
                }

              printf("[MASTER] Local partition: %d \n",local_partition);
              fclose(fp);
              memset(path_to_read,0,800);
 
           ind++;
       }

        /*Displaying result of master*/

        pCounter = pStart;
        while(pCounter != NULL){
            show(pCounter,rank);
            pCounter = pCounter -> pNext;
        }
        printf("\n \n \n");

    }/* endif rank=0*/

    
        else{   
            
            
            FILE *file;
            
            char temp_to_send [7800000];
            memset(temp_to_send,0,7800000);
            int cum_sum = 0,start =0,while_counter = 0;
            MPI_Recv(&numberfile,1,MPI_INT,0,99,MPI_COMM_WORLD,&status);
            char file_name_and_number_words[numberfile][100];
            int how_much_word [numberfile];
            MPI_Recv(file_name_and_number_words,sizeof(file_name_and_number_words)/sizeof(file_name_and_number_words[0][0]),MPI_CHAR,0,0,MPI_COMM_WORLD,&status);
            MPI_Recv(how_much_word,sizeof(how_much_word)/sizeof(how_much_word[0]),MPI_INT,0,1,MPI_COMM_WORLD,&status);
            MPI_Recv(&partition,1,MPI_INT,0,2,MPI_COMM_WORLD,&status);
            MPI_Recv(&remainder,1,MPI_INT,0,3,MPI_COMM_WORLD,&status);
            printf("Im the slave with rank: %d \n",rank);
            fflush(stdout);
           

            if ( remainder !=0 ){

                if (rank<remainder){  // occhio forse dovrebbe essere <=
                    lowerbound = (partition + 1) * rank;
                    upperbound = (partition+1) * (rank+1);
                    partition++;
                }else{
                    lowerbound = (partition*rank)+remainder;
                    upperbound = (partition * (rank+1))+remainder;
                }
            } else { /* non c'è resto*/
                lowerbound = partition*rank;
                upperbound = partition * (rank+1);
            }
           
           printf("[RANK %d] Ho ricevuto una partition di: %d e un resto di: %d \n",rank,partition,remainder);
           printf("[RANK %d] Lowrbound is: %d \n",rank,lowerbound);
           fflush(stdout);
           
            for(int tmp=0;tmp<sizeof(how_much_word)/sizeof(how_much_word[0]);tmp++){
                cum_sum += how_much_word[tmp];
                printf("[RANK %d] Cumulative sum: %d \n",rank,cum_sum);
                fflush(stdout);

                if((cum_sum > lowerbound) &&( partition > 0 )){
                    strcpy(path,"myfolder/");
                    strcat(path,file_name_and_number_words[tmp]);
                    printf("[RANK %d] Sta aprendo: %s \n",rank,path);
                    fflush(stdout);
                    file = fopen(path,"r");
                    if(file == NULL){
                        perror("Error with opening");
                    }
                   
                    start = how_much_word[tmp] - (cum_sum-lowerbound);
        
                    if (start < 0){
                        start = 0;
                    }

                    printf("[RANK %d] Inizio a leggere dalla posizione: %d \n",rank,start);
                    fflush(stdout);

                    while((ch = fgetc(file)) != EOF){
                        
                        if(isalnum(ch)!=0 ){
                            in_word =1;
                            if(word_count>=start){
                                ch = tolower(ch);
                                tmpword[index_of_tmpword] = ch;
                                index_of_tmpword++; 
                                num_car++;
                                temp_to_send[while_counter] = ch;
                                while_counter++;
                            }

                        } else{
                            if ((ch == ' ' || ch == '\t' || ch== '\n') && (in_word==1)){
                                word_count++;
                                in_word=0;
                                if(word_count > start){
                                    partition--;
                                    tmpword[index_of_tmpword] ='\0';
                                    index_of_tmpword++;
                                    addWord(tmpword);
                                    memset(tmpword,0,200);
                                    num_car++;
                                    index_of_tmpword = 0;
                                    temp_to_send[while_counter] = 0;
                                    while_counter++;
                                }
                                if(partition <0){
                                    break;
                                }
                            }/*fine if tabulazione*/
                        } /*fine else*/
                    } /*fine while*/
                    printf("[RANK %d] Mi sono fermato con il while, ora la partition è: %d \n",rank,partition);
                    fflush(stdout);
                    
                } /*fine if*/
                memset(path,0,2100);
                
            }/*fine for*/
            
            fclose(file);
            
        
       

        /*for (int w =0 ;w<1000;w++){
            if(temp_to_send[w]== 0){
                printf("=00000000");
            }
            printf("%c ",temp_to_send[w]);
        }*/
        exactly_word = malloc(sizeof(char)*num_car);   //ok
        printf("NUM CAR %d \n", num_car);             //ok
        
        for(int ex = 0; ex<num_car; ex++){
            if(temp_to_send[ex] != 0 ) {
                exactly_word[ex] = temp_to_send[ex];           //ok
            }else{
                
                exactly_word[ex] = 0;
            }
        }
        
        for(int w = 0; w <num_car; w++){
            
            printf("%c",exactly_word[w]);
                                    //ok
            
        }
        
       
      
        readed_nd_word = number_non_duplicate_words();
        printf("$$[RANK %d] Ha un numero di parole non duplicate pari a: %d",rank,readed_nd_word);
        fflush(stdout);
        
        pCounter = pStart;
        
        while(pCounter != NULL){
            // calcola lunghezza della stringa e passala
            // invia la parola e poi il contatore
            //incrementa il tag
            show(pCounter,rank);
            pCounter = pCounter -> pNext;
        }
        printf("\n \n \n");
             
    } /*Fine codice slave*/

    MPI_Allgather(&num_car,1,MPI_INT,&recv,1,MPI_INT,MPI_COMM_WORLD);
    MPI_Gather(&readed_nd_word,1,MPI_INT,&recvs_counts,1,MPI_INT,0,MPI_COMM_WORLD);
    

    if (rank == 0){
        for (int j = 0; j< world_size;j++){
            printf("[RANK %d] ricevuto %d caratteri totali \n",rank,recv[j]);             //ok 0 e 9
        }
        printf("IL MASTER HA RICEVUTO DALLA GATHER IL VALORE DELLE PAROLE CONTATE DA 1 SENZA DUPLICATI: %d",recvs_counts[1]);
    }   
    
    int sec_size [world_size];
    int num = 0;


    for (int k = 0; k<world_size;k++){
        
        sec_size[k] = recv[k];
        disp[k] = k == 0 ? 0: disp[k-1]+sec_size[k-1];
        num += sec_size[k];
        }
        

    for (int x= 0 ; x < world_size; x++){
        printf("[Rank %d] DISPLACMENT: %d E SEC SIZE: %d \n",rank,disp[x],sec_size[x]);
    }
    

    

    if(rank == 0){
            printf("IF NEL RANK 0 prima  dellA MALLOC num è pari a %d \n",num);
            result_word = malloc(sizeof(char)* num);
           
            printf("DOPO LA MALLOC \n");
            exactly_word = malloc(sizeof(char)*0);

            printf("numcar di rank 0  %d", num_car);
            
            

            
    }
    
    
    MPI_Gatherv(exactly_word,num_car,MPI_CHAR,result_word,sec_size,disp,MPI_CHAR,0,MPI_COMM_WORLD);  
    
    
    if(rank==0){
        char array[num];
        memcpy(array,result_word,num);
        printf("@@@@ %s ", result_word);
        for ( int n = 0 ; n < num; n++){
            printf("Carattere %c ",array[n]);
            if (array[n] == 0){
                printf("trovato null");
            }
        }
        
    }
    
  
        
         
    
    MPI_Finalize();
    return 0;

}