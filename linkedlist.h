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
void show(struct WordCounter *pWordcounter);        /* Outputs a word and its count of occurrences */
struct WordCounter* createWordCounter(char *word);