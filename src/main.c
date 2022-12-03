#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include "nfa.h"
#include "dfa.h"

int main(int argc, char *argv[]) {
    FILE* input_ptr = fopen("input.txt" , "r");
    int n; 
    fscanf(input_ptr ,"%d" , &n);
    char input[500];
    fscanf(input_ptr ,"%s" , input);
    fclose(input_ptr);
    int sz = strlen(input);
    pid_t p = fork();
    if(p > 0) {
        wait(NULL);
        minimise_dfa(n);
    } else {
        startNFA(n , input , sz);
    }
    
    return 0;
}