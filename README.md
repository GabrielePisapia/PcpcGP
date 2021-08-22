# MPI WORD-COUNT CONCURRENT AND PARALLEL PROGRAMMING 2020/2021 

The following project involves the implementation of a parallel and concurrent solution for the word count problem, realized through open mpi in c. Realized by the student: Pisapia Gabriele.

# PROBLEM STATEMENT

The problem consists of reading a number of variable files, containing a random number of words, in order to read the words and count all occurrences in each file and for each word. Specifically, this task is realized through principles of parallel programming, so each processor involved has a portion of the file to analyze and to communicate the results with the other processors involved in the presentation of the solution. 
To provide a solution, we need to decompose the problem into 3 subproblems, which are:
- Partition the number of words to be counted for each processor fairly and correctly, making sure that each processor reads the same number of words, or at most, in the case of any remainder one extra word.
- Perform the word count of each word detected in the files of interest for each processor and make a histogram containing the results.
- Communicate to the master process all the results obtained by the single processors, and join the occurrences found by the different processors, enlarging the histogram of the master process.

# FUNDAMENTAL ELEMENTS

For the local histogram, I used a linked list that is able to maintain a list of the words detected in each file and their relative counts. The structure is very simple, the words and counts are linked sequentially and methods are exposed for: scrolling the list, inserting a new item, updating the counters and other utility methods.
The structure is defined as follows:

```
struct WordCounter
{
  char *word;
  int word_count;
  struct WordCounter *pNext;
};
```

```
/*Adds a word to the list or update*/
void addWord(char *pWord);  

/*show the list*/
void show(struct WordCounter *pWordcounter,int rank); 

/* Creates a new WordCounter structure */
struct WordCounter* createWordCounter(char *word,int number);  

/*Count how much word there are in the struct (non duplicate words)*/
int number_non_duplicate_words(); 

/*give the counter of the current word pointed*/
int giveCounter();

/*give the length of the current word pointed */
int lengthOfCurrentWord(struct WordCounter *pWordCounter)

/* return the current word pointed */
char* giveWord(struct WordCounter *pWordcounter)

/* Method for update and merge words through processes */
void addOrIncrement(char *word, int her_count)
```

**The next sections will illustrate the solutions to the three subproblems listed above**

# Partitioning solution

The first thing we need is to know how many words there are in total in the set of files considered. This is very trivial, it is sufficient to make a reading of every single file and to count its content. Each word is considered as the beginning of a character, the characters following the first one and obviously a tab character for the end (\n \t ,etc...).

**MASTER PARTITION**

Once the total number of words is obtained, the partition is calculated based on the number of processors involved to perform the job. This is simply expressed as a division, without considering the decimal part. Once this value is calculated, the remainder must be identified, to figure out which processes should take one more word than the full partition. This is true since the remainder is always between 0 and the number of processors n ( 0 <= r <= n).

After calculating these values, the first communication between the processes begins. In particular, the master sends to the slaves the partition, the remainder and other important information, such as the list of file names and the number of words contained in each of them.
After the values are communicated, the master starts processing its portion of the task. From this we can derive 2 situations:
1) The first file has fewer words than its partition, so you need to read it all and continue with the next file.
2) The first file has more words than its partition, so it must read at most partition words.

**SLAVE PARTITION**

As for the slaves, the situation is slightly more complicated, since they have to figure out from which word they have to start performing the count. Consequently they have to understand which file to read and from which position to start, because the previous process may not have read all the file, but only a small part.
The first thing is to figure out what is the lower bound of the words to be read, basically what word should he start reading from. And this is accomplished through these simple calculations:

 ```
  if ( remainder !=0 ){
        if (rank<remainder){  
            lowerbound = (partition + 1) * rank;
            partition++;
            }else{
                lowerbound = (partition*rank)+remainder;
                }
            } else { 
                lowerbound = partition*rank;
            }
    }
```

Once we understand the lower bound, we can begin to identify the exact location to start reading from.
There are several steps:
1) Identify exactly which file to read, to do this we can use a variable that is incremented in a loop by the number of words in the i-th file in the loop. When this variable exceeds the calculated lower bound, it means that we have found the file to start reading from.

```
for(int tmp=0;tmp<sizeof(how_much_word)/sizeof(how_much_word[0]);tmp++){
     cum_sum += how_much_word[tmp];   // how_much_word contains the number of word inside the tmp index file.
    if((cum_sum > lowerbound) &&( partition > 0 )){
        ...
        /*Ok this is the file that we were looking for*/
        ...   
    }
}
```

2) Now we need to figure out if this file was read in full by the previous process or still contains words to be considered and then resume counting exactly where the previous process stopped. 
 To do this I used a start variable calculated as the difference between the number of words in the current file from which we subtract the difference between the cumulative sum and the lowerbound. This can have two outcomes: if start is less than 0, it means that the current file must be read from the beginning, otherwise if start is positive its result is exactly the point from which to start reading. For example if start is 100, process p will start reading from the 101st word.

```
start = how_much_word[tmp] - (cum_sum-lowerbound);
        if (start < 0){
            start = 0;
        }
```

