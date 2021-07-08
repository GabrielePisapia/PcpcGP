
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wordcounter.h"

struct WordCounter *pStart = NULL; 

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