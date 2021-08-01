#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

struct WordCounter *pStart = NULL; 

struct WordCounter
{
  char *word;
  int word_count;
  struct WordCounter *pNext;                        /* Pointer to the next word counter in the list */
};

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
}

char* giveWord(struct WordCounter *pWordcounter){
    return pWordcounter->word;
}



int main(int argc, char **argv){

        DIR *d;
        FILE *fp;    
        int ch;
        int num_car = 0;
        int in_word = 0, index_of_tmpword = 0,word_count = 0;
        char tmpword[100];
        printf("hello");
        fflush(stdout);
        char temp_to_send[7800000];
        memset(temp_to_send,0,7800000);
        int while_counter = 0;
        struct dirent *dir;
        struct WordCounter *pCounter = NULL;
        MPI_Status status;
        MPI_Init(&argc,&argv);
        int world_size,rank;
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        MPI_Comm_rank (MPI_COMM_WORLD,&rank);
        int recv[world_size];
        
        printf("%d \n world size",world_size);
        fflush(stdout);
        printf("Prima dell'apertura della dir \n");
        fflush(stdout);
            
            d=opendir("myfolder");
                if(d){
                        
                        fp = fopen("myfolder/file2.txt","r");
                        
                        while((ch = fgetc(fp)) != EOF){

                        if(isalnum(ch)!=0 ){
                            in_word =1;
                            ch = tolower(ch);
                            tmpword[index_of_tmpword] = ch;
                            index_of_tmpword++; 
                            num_car++;
                            temp_to_send[while_counter] = ch;
                            while_counter++;

                        } else{
                            if ((ch == ' ' || ch == '\t' || ch== '\n') && (in_word==1)){
                                word_count++;
                                in_word=0;
                                tmpword[index_of_tmpword] ='\0';
                                num_car++;
                                index_of_tmpword++;
                                addWord(tmpword);
                                memset(tmpword,0,100);
                                index_of_tmpword = 0;
                                temp_to_send[while_counter] = '\0';
                                while_counter++;
                                
                            }/*fine if tabulazione*/
                        } 
                        
                    }
                }else{
                    perror("erroe");
                }
            fclose(fp);
            closedir(d);
   
    pCounter = pStart;
    printf("Num car: %d \n",num_car);

    MPI_Gather(&num_car,1,MPI_INT,&recv,1,MPI_INT,0,MPI_COMM_WORLD);

    if (rank == 0){
      printf("Ho ricevuto: %d caratteri dal processo [%d]  \n",recv[1],1);
      printf("Ho ricevuto: %d dal processo [%d]",recv[2],2);
    }

/*
    int disp[world_size];
    
    for (int k = 0; k<world_size;k++){
      disp[k] = recv[k]+1;
    }*/

    if (rank == 1){
      for (int i = 0;i<sizeof(temp_to_send)/sizeof(temp_to_send[0]);i++){
        if(temp_to_send[i] != 0){
          printf("%c ",temp_to_send[i]);  
          }
        } 
    }
    
    MPI_Finalize();
    return 0;
  
}

/*
    char *wordy;
    pCounter = pStart;
    wordy = giveWord(pCounter);
    */