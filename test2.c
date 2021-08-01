/*
 * C Program to List Files in Directory
 */
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define TRUE                1
#define FALSE               0

struct WordCounter
{
  char *word;
  int word_count;
  struct WordCounter *pNext;                        /* Pointer to the next word counter in the list */
};

/* Function prototypes */
void addWord(char *pWord);                          /* Adds a word to the list or updates exisiting word */
int is_separator(char ch);                          /* Tests for a separator character */
void show(struct WordCounter *pWordcounter);        /* Outputs a word and its count of occurrences */
struct WordCounter* createWordCounter(char *word);  /* Creates a new WordCounter structure */

/* Global variables */
struct WordCounter *pStart = NULL;                  /* Pointer to first word counter in the list */
 
int main(void)
{
    int word_count = 0, in_word = 0;
    //int isfirst = 0;
    int ch;
    DIR *d;
    FILE *fp;
    char path[2100];
    char tmpword[250];
    char buffer[200];
    int index_of_tmpword = 0,len=0;
    struct WordCounter *pCounter = NULL;
    struct dirent *dir;
    d = opendir("myfolder");
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
            fflush(stdout);
            
                strcpy(path,"myfolder/");
                //strcat(path,"/");
                strcat(path,dir->d_name);
                fp = fopen(path, "r");

                if(fp == NULL) {
                    perror("Could not open the file");
                    }
                    while ((ch = fgetc(fp)) != EOF) {
                      
                       if (isalnum(ch)!= 0 ){
                           in_word = 1;  
                           tmpword[index_of_tmpword] = ch;
                           index_of_tmpword++;   /*se è un carattere alza il flag -> si abbassa solo quando legge una tabulazione e ha precedentemente letto una lettera*/
                       }
                       else{
                           if ((ch == ' ' || ch == '\t' || ch== '\n') && (in_word==1)){
                               word_count++;
                               in_word= 0;
                               printf("aggiornato wc");
                               printf("%d",in_word);
                               tmpword[index_of_tmpword] = '\0';
                               index_of_tmpword++;
                               addWord(tmpword);
                               for(int j=0;j<150;j++){
                                tmpword[j]='\0';
                                }
                                index_of_tmpword=0;      /* Add the word to the list */
                              }
                            } /*fine dell'else*/
                

                    }/*fine while lettura caratteri*/
                              
        } //fine while directory
    } //fine if d*/
    fclose(fp);
    closedir(d);

    pCounter = pStart;
    while(pCounter != NULL)
    {
    show(pCounter);
    pCounter = pCounter->pNext;
    }
  printf("\n");

  /* Free the memory that we allocated */
  pCounter = pStart;
  while(pCounter != NULL)
  {
    free(pCounter->word);        /* Free space for the word */
    pStart = pCounter;           /* Save address of current */
    pCounter = pCounter->pNext;  /* Move to next counter    */
    free(pStart);                /* Free space for current  */     
  }

    printf("%d number of total words",word_count);
    return(0);

}// finemain
    


/* Returns TRUE if the argument is a separator character and FALSE otherwise */
int is_separator(char ch)
{
  /* Separators are space, comma, colon, semicolon, double quote, question mark, exclamation, and period */
  static char separators[] = { ' ' , ',',':' , '\"', '?' , '!' , '.'};
  int i = 0;

  for(i = 0 ; i<sizeof separators ; i++)
  {
    if(ch == separators[i])
      return TRUE;
  }

  return FALSE;
}

void show(struct WordCounter *pWordcounter)
{
  /* output the word left-justified in a fixed field width followed by the count */
  printf("\n%-30s   %5d", pWordcounter->word,pWordcounter->word_count);
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

