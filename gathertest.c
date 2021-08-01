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

struct WordAndCount
{
  char *parola;
  int contatore;
};

/* Function prototypes */
void addWord(char *pWord);                          /* Adds a word to the list or updates exisiting word */
int is_separator(char ch);                          /* Tests for a separator character */
void show(struct WordCounter *pWordcounter,int rank);        /* Outputs a word and its count of occurrences */
struct WordCounter* createWordCounter(char *word);  /* Creates a new WordCounter structure */

/* Global variables */
struct WordCounter *pStart = NULL;                 /* Pointer to first word counter in the list */

/* Implementation of functions for the data struct */

void show(struct WordCounter *pWordcounter,int rank)
{
  /* output the word left-justified in a fixed field width followed by the count */
  printf("\n [RANK %d]: %s   %d",rank, pWordcounter->word,pWordcounter->word_count);
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

    struct WordCounter *pCounter = NULL;
    MPI_Status status;
    MPI_Init(&argc,&argv);
    int world_size,rank,end,i,start =0,numberfile=0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank (MPI_COMM_WORLD,&rank);
    MPI_Datatype linkedlist,oldtype[1];
    MPI_Aint offset[1],lw,extent;
    int blockcounts[1];

    offset[0] = 0;
    oldtype[0] = MPI_CHAR;
    blockcounts[0] = 1;

    MPI_Type_get_extent(MPI_CHAR,&lw,&extent);
    offset[1] = extent;
    oldtype[1] = MPI_INT;
    blockcounts[1] = 1;
    MPI_Type_create_struct(2,blockcounts,offset,oldtype,&linkedlist);
    MPI_Type_commit(&linkedlist);

    if(rank==0){
      printf("HELLO");
      fflush(stdout);
      struct WordAndCount countering;
      char x[100];
      strcat(x,"a");
      strcat(x,"b");
      strcat(x,"\0");
      printf("PRIMA DI TUTTO");
      fflush(stdout);
      printf("%s \n",x);
      fflush(stdout);

      countering.parola = x;
      //printf("%s \n",countering.parola);
    }

}