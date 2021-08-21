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
struct WordCounter* createWordCounter(char *word,int number);  /* Creates a new WordCounter structure */
int number_non_duplicate_words();                    /*Count how much word there are in the struct (non duplicate words)*/
int giveCounter();


/* Global variables */
struct WordCounter *pStart = NULL;                 /* Pointer to first word counter in the list */

/* Implementation of functions for the data struct */

void show(struct WordCounter *pWordcounter,int rank)
{
  /* output the word left-justified in a fixed field width followed by the count */
  printf("\n [RANK %d]: %s   %d",rank, pWordcounter->word,pWordcounter->word_count);
}

int giveCounter (struct WordCounter *pCounter){
    int tempcount = pCounter -> word_count;
   return tempcount;
    
}

int lengthOfCurrentWord(struct WordCounter *pWordCounter)
{
    int ind = 0;
    int len = 0;
    char array[100]; 
    strcat(array, pWordCounter -> word);
    /*while (array[ind] != 0){
        len++;
        ind++;
        
    }
    memset(array,0,100);*/
    return strlen(pWordCounter->word)+1;  // for the null terminator string (probably an error)
}

char* giveWord(struct WordCounter *pWordcounter){  // return current word of the linked list
    return pWordcounter->word;
}

int number_non_duplicate_words(){ 

    struct WordCounter *pCounter = NULL;
    pCounter = pStart;
    int returnvalue = 0;
    while(pCounter != NULL){
        returnvalue++;
        pCounter = pCounter->pNext;
    }
    return returnvalue;
}

void addOrIncrement(char *word, int her_count){ // Merge words, increment if exist or add to data struct if not

  struct WordCounter *pCounter = NULL;
  struct WordCounter *pLast = NULL;
 

   pCounter = pStart;
   while(pCounter != NULL){
       if(strcmp(word, pCounter-> word) == 0){ 
           int old_count = pCounter -> word_count;
           int new_count = old_count+her_count;
           pCounter -> word_count = new_count;
           return;
       }
        
        pLast = pCounter;            /* Save address of last in case we need it */
        pCounter = pCounter->pNext;
   }
   pLast->pNext = createWordCounter(word,her_count);
  
}

int checkIfExist(char *word){
  struct WordCounter *pCounter = NULL;
  struct WordCounter *pLast = NULL;
  pCounter = pStart;
  while(pCounter != NULL){
      if(strcmp(word,pCounter->word)== 0){
          return 1;
      }
      pCounter = pCounter->pNext;
  }
  return 0;
}

void addWord(char *word)
{
  struct WordCounter *pCounter = NULL;
  struct WordCounter *pLast = NULL;

  if(pStart == NULL)
  {
    pStart = createWordCounter(word,1);
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
  pLast->pNext = createWordCounter(word,1);
}

/* Create and returns a new WordCounter object for the argument */
struct WordCounter* createWordCounter(char *word,int number)
{
  struct WordCounter *pCounter = NULL;
  pCounter = (struct WordCounter*)malloc(sizeof(struct WordCounter));
  pCounter->word = (char*)malloc(strlen(word)+1);
  strcpy(pCounter->word, word);
  pCounter->word_count = number;
  pCounter->pNext = NULL;
  return pCounter;
}

int main (int argc,char *argv[]){
    
    /*Useful variables init*/
    int rank,partition,remainder,in_word=0,how_much_file;
    int lowerbound =0 ,local_partition =0,word_count=0,local_wc=0; int readed_nd_word = 0;
    int num_car =0;
    double timestart,timend;
    

       

    char path[2100];
    int ch,index_of_tmpword=0;
    char tmpword[500];
    struct WordCounter *pCounter = NULL;
    char *result_word;
    char *exactly_word;
    int *counters;
    int *total_counters;

    MPI_Status status;
    MPI_Init(&argc,&argv);
    int world_size,chunk,end,i,start =0,numberfile=0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    MPI_Barrier(MPI_COMM_WORLD);
    timestart = MPI_Wtime();
    int recv[world_size];
    int disp[world_size];
    int num_disp[world_size];
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
            lowerbound = 0; /*Da che parola deve iniziare a leggere*/
            local_partition = partition+1;
       }else{ // divisione equa
            lowerbound = 0;
            local_partition = partition;
       }


       int ind = 0;
       char path_to_read [800];
       
       

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
                                memset(tmpword,0,500);
                                index_of_tmpword = 0;
                                if(local_partition<=0){
                                    break;
                                }
                            }/*fine if tabulazione*/
                        } 
                }

              
              fclose(fp);
              memset(path_to_read,0,800);
 
           ind++;
       }

        /*Displaying result of master*/
/*
        pCounter = pStart;
        while(pCounter != NULL){
            show(pCounter,rank);
            pCounter = pCounter -> pNext;
        }
        printf("\n \n \n");*/

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

           

            if ( remainder !=0 ){

                if (rank<remainder){  
                    lowerbound = (partition + 1) * rank;
                    partition++;
                }else{
                    lowerbound = (partition*rank)+remainder;
                }
            } else { /* non c'è resto*/
                lowerbound = partition*rank;
            }
           
           

           
            for(int tmp=0;tmp<sizeof(how_much_word)/sizeof(how_much_word[0]);tmp++){
                cum_sum += how_much_word[tmp];
                fflush(stdout);

                if((cum_sum > lowerbound) &&( partition > 0 )){
                    strcpy(path,"myfolder/");
                    strcat(path,file_name_and_number_words[tmp]);
                    fflush(stdout);
                    file = fopen(path,"r");
                    if(file == NULL){
                        perror("Error with opening");
                    }
                   
                    start = how_much_word[tmp] - (cum_sum-lowerbound);
        
                    if (start < 0){
                        start = 0;
                    }

                    
                    fflush(stdout);

                    while((ch = fgetc(file)) != EOF){
                        
                        if(isalnum(ch)!=0 ){
                            in_word =1;
                            if(word_count>=start){
                                ch = tolower(ch);
                                tmpword[index_of_tmpword] = ch;
                                index_of_tmpword++; 
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
                                    memset(tmpword,0,500);
                                    index_of_tmpword = 0;
                                    temp_to_send[while_counter] = 0;
                                    while_counter++;
                                }
                                if(partition <=0){
                                    break;
                                }
                            }/*fine if tabulazione*/
                        } /*fine else*/
                    } /*fine while*/
                    fflush(stdout);
                    
                } /*fine if*/
                memset(path,0,2100);
                
            }/*fine for*/
            
            fclose(file);
      
        readed_nd_word = number_non_duplicate_words();
        counters = malloc(sizeof(int)*readed_nd_word);
    

        char wordy[100];
        

        pCounter = pStart;
        while(pCounter != NULL){
            num_car += lengthOfCurrentWord(pCounter);
            pCounter = pCounter -> pNext;
            fflush(stdout);  

        }
        
        exactly_word = malloc(sizeof(char)*num_car);
        memset(exactly_word,0,num_car);
        
        pCounter = pStart;
        int indice = 0;
        int indice_parole = 0;
        for(int x=0; x<readed_nd_word;x++){
            counters[x] = giveCounter(pCounter);
            strcpy(wordy,giveWord(pCounter));
            //printf("WORDY: %s \n",wordy);
           
            while(wordy[indice]!= 0){
                exactly_word[indice_parole] = wordy[indice];
                indice_parole++;
                indice++;
            }
            
            pCounter = pCounter -> pNext;
            indice = 0;
            exactly_word[indice_parole] =0;
            indice_parole++;
            
            
           // printf("[RANK %d] Counters di %d è uguale a %d \n",rank,x,counters[x]);
        }

        //printf("Carattere: %c \n",exactly_word[0]);

        pCounter = pStart;
        /*
        while(pCounter != NULL){
            show(pCounter,rank);
            pCounter = pCounter -> pNext;
        }
        printf("\n \n \n");*/

        /* Free the memory that we allocated */
        pCounter = pStart;
        while(pCounter != NULL)
        {
            free(pCounter->word);        /* Free space for the word */
            pStart = pCounter;           /* Save address of current */
            pCounter = pCounter->pNext;  /* Move to next counter    */
            free(pStart);                /* Free space for current  */     
        }
             
    } /*Fine codice slave*/


    MPI_Allgather(&num_car,1,MPI_INT,&recv,1,MPI_INT,MPI_COMM_WORLD);  // numero di caratteri letti
    MPI_Allgather(&readed_nd_word,1,MPI_INT,&recvs_counts,1,MPI_INT,MPI_COMM_WORLD);  // numero di conteggi delle parole (non duplicate)
   

    
    int sec_size [world_size];
    int sec_count_size [world_size];
    int num_count = 0;
    int num = 0;

    /* Preparing parameters for gatherv questo deve inviare i conteggi */
    
    for (int y = 0; y<world_size;y++){
        
        sec_count_size[y] = recvs_counts[y];
        num_disp[y] = y == 0 ? 0: num_disp[y-1]+sec_count_size[y-1];
        num_count += sec_count_size[y];
        }
        
    for (int k = 0; k<world_size;k++){
        
        sec_size[k] = recv[k];
        disp[k] = k == 0 ? 0: disp[k-1]+sec_size[k-1];
        num += sec_size[k];
        }
        
  
    

    

    if(rank == 0){
    
            result_word = malloc(sizeof(char)* num);
            exactly_word = malloc(sizeof(char)*0);
            counters = malloc(sizeof(int)*0);
            total_counters = malloc(sizeof(int)*num_count);
            
           
            
            
    }

    MPI_Gatherv(exactly_word,num_car,MPI_CHAR,result_word,sec_size,disp,MPI_CHAR,0,MPI_COMM_WORLD);
    MPI_Gatherv(counters,readed_nd_word,MPI_INT,total_counters,sec_count_size,num_disp,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    timend = MPI_Wtime();

    if(rank==0){
        
        
        pCounter = pStart;
        char array[num];
        char tmp_word[100];
        int index_of_word_count = 0;
        int count_parole = 0;
        memcpy(array,result_word,num);
    
        for ( int n = 0 ; n < num; n++){
            if (result_word[n] == 0){
                

                addOrIncrement(tmp_word,total_counters[count_parole]);
                memset(tmp_word,0,100);
                index_of_word_count = 0;
                count_parole++;
            }else{
                tmp_word[index_of_word_count] = result_word[n];
                index_of_word_count++;
            }
        }

        pCounter = pStart;
        /*
        while(pCounter != NULL){
            show(pCounter,rank);
            pCounter = pCounter -> pNext;
        }
        printf("\n \n \n");*/
        
        
        /*Writing the all words and their occurrence in the csv file*/
        pCounter = pStart;
        FILE *file;
        file = fopen("result_word_count.csv","w+");
        fprintf(file,"WordCount Procject 2020/2021 by Pisapia Gabriele \n");

        while(pCounter != NULL){
            fprintf(file,"%s,%d \n", giveWord(pCounter),giveCounter(pCounter));
            pCounter = pCounter ->pNext;
        }
        fclose(file);
        printf("%f \n ",timend-timestart);
        fflush(stdout);
    }
    
  
        
         
    free(result_word);
    free(exactly_word);
    free(counters);
    free(total_counters); 
    MPI_Finalize();
    return 0;

}