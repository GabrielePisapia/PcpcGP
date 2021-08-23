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

    ...

    while(partition != 0){
    if (word_count > start){
        /* perform word counting */
    }

    ...

    }
```

# LOCAL WORD COUNTING SOLUTION

Whenever the end of a new word is reached, a temporary array containing the characters of that word is filled, so that it can be added to the linked list and then reset and re-used for the next word.

```
else{
        if ((ch == ' ' || ch == '\t' || ch== '\n') && (in_word==1)){

            /* We have reached the end of a word */

            word_count++;
            in_word=0;
            if(word_count > start){
                partition--;
                tmpword[index_of_tmpword] ='\0';  // this contains the word to add 
                index_of_tmpword++;
                addWord(tmpword);  // add to the linked list
                memset(tmpword,0,500);
                index_of_tmpword = 0;
                temp_to_send[while_counter] = 0;
                while_counter++;
                }
            if(partition <=0){
                break;
                }
        }
} 
```

At this point, each process has generated its own local histogram and is ready to communicate the words and counts to the master.

# COMMUNICATE THE RESULT TO THE MASTER

The solution to this problem is very complex, since we know very well that within a communication with mpi, it is not possible to send pointers, because these would point to different memory locations for each processor. Therefore we need to find a way to communicate the data structure in a simple way.
The approach I used is to put in a very large array the set of all detected words separated by the character \0. The same principle was applied for the counts in order to "synchronize" the two arrays in the master and to reconstruct the linked list.
First of all we understand the size of the array containing all the words, to do this I counted the number of characters contained in the local linked list and I defined a variable useful for this purpose.

```
counters = malloc(sizeof(int)*readed_nd_word);  // array of the counters
pCounter = pStart;
    while(pCounter != NULL){
        num_car += lengthOfCurrentWord(pCounter);
        pCounter = pCounter -> pNext;
    }

    exactly_word = malloc(sizeof(char)*num_car);

    /* fill the two arrays */
    
```

Now we can start the communication with the master, for doing this I used a gatherv, before doing this we need some parameters for the gather, such as the displascment and the size of the size to send.

```
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
        
```

N.B. the master does not participate in the calculation of displacment and size, so its parameters are set to 0.
Let's start the communication:

```
MPI_Gatherv(exactly_word,num_car,MPI_CHAR,result_word,sec_size,disp,MPI_CHAR,0,MPI_COMM_WORLD);

MPI_Gatherv(counters,readed_nd_word,MPI_INT,total_counters,sec_count_size,num_disp,MPI_INT,0,MPI_COMM_WORLD);
```

# MERGE THE RESULT INTO MASTER'S HISTOGRAM

We have now in a one big array all the words derived from the other process, so we can loop and reconstruct the linked list.

        for ( int n = 0 ; n < num; n++){
            if (result_word[n] == 0){
                addOrIncrement(tmp_word,total_counters[count_parole]);  // merge or insert the word
                memset(tmp_word,0,100);
                index_of_word_count = 0;
                count_parole++;
            }else{
                tmp_word[index_of_word_count] = result_word[n];  // we copy character by character
                index_of_word_count++;
            }
        }

The addOrIncrement method takes in input the words and understands if the master has already that word and consequently it must sum its occurrence with that of the slave, taking the count from the array of counts. If it does not have that word it must add it without adding, but inserting as count the one detected locally by the slaves.

# WRITING RESULT INTO THE CSV FILE

At this point master has got a big linked list which contains all words and all counts, and can iterate through the list to write the results into the csv file.

```
pCounter = pStart;
FILE *file;
file = fopen("result_word_count.csv","w+");
fprintf(file,"WordCount Procject 2020/2021 by Pisapia Gabriele \n");
    while(pCounter != NULL){
        fprintf(file,"%s,%d \n", giveWord(pCounter),giveCounter(pCounter));
        pCounter = pCounter ->pNext;
    }
fclose(file);
```

# BENCHMARKING

The proposed solution was tested on a cluster of aws EC2 t2.2xlarge machines with the following features: 8 vCPU and 32Gb of RAM. The benchmarking metrics have been obtained considering the concepts of weak and strong scalability.
The tests were repeated several times and from the results produced, an average calculation was performed, to define a more or less accurate value for the specific test case.

**STRONG SCALABILITY**

A total of 100,000 words spread across multiple files was considered for strong scalability.
To analyze the performance behavior of the program, I linearly increased the number of processors involved in the execution of the task.
