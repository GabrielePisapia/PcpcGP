#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc,char *argv[]){
  

char x[2000000];
memset(x,0,2000000);
x[0] = 'c';
x[1] ='i';

for (int i = 0;i<sizeof(x)/sizeof(x[0]);i++){
  if(x[i] != 0){
    printf("%c",x[i]);  
  }
}

}

