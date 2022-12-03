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

int n , shmid , length , id = 0;
int *flagPtr;
char inp[500];
FILE* output_task1;

bool isValidState(struct state cur_state) {
    return cur_state.x >= 0 && cur_state.y >= 0 && cur_state.x <= n && cur_state.y <= n;
}

int vis_states[500];

void Transition_Function(struct state cur) {
    if(flagPtr[0])
        return;
    fprintf(output_task1 ,"[%d:%d] " , getppid() , getpid());
    fprintf(output_task1 , "Exploring Path: ");
    for(int i = 0 ; i <= id; i++) 
        fprintf(output_task1 , "%d " ,  vis_states[i]);
    fprintf(output_task1 , "\n");
    fflush(output_task1);
    if(id == length) {
        if(cur.x == n && cur.y == n) {
            flagPtr[0] = true;
            fprintf(output_task1 , "Accepted! Followed path: ");
            for(int i = 0 ; i <= length ; i++)
                fprintf(output_task1 , "%d " , vis_states[i]);
            fprintf(output_task1 , "\n");
            fflush(output_task1);
            return;
        }
        fprintf(output_task1 , "Failed at Path: ");
        for(int i = 0 ; i <= length ; i++)
            fprintf(output_task1 , "%d " , vis_states[i]);
        fprintf(output_task1 , "\n");
        fflush(output_task1);
        return;
    }

    pid_t subp = fork();
    if(subp > 0) {
        if(inp[id] == '0') 
            cur.x--;
        else 
            cur.y--;
        if(isValidState(cur)) {
            id++;
            vis_states[id] = cur.y * (n + 1) + cur.x;
            Transition_Function(cur);
        }
        wait(NULL);
    } else {
        if(inp[id] == '0') 
            cur.x++;
        else 
            cur.y++;
        if(isValidState(cur)) {
            id++;
            vis_states[id] = cur.y * (n + 1) + cur.x;
            Transition_Function(cur);
        }
    }
}

void startNFA(int sz , char ip[] , int len) {
    n = sz;
    for(int i = 0 ; i <= len ; i++)
        inp[i] = ip[i];
    length = len;
    pid_t p = fork();
    output_task1 = fopen("2020A7PS1080H_t1.txt" , "w+");
    if(p > 0) {
        while(wait(NULL) > 0);
        shmid = shmget(1080, 8 , 0666);
        flagPtr = shmat(shmid , 0 , 0);
        shmctl(shmid , IPC_RMID , NULL);
        fclose(output_task1);
    } else {
        struct state initial;
        initial.x = 0 , initial.y = 0;
        id = 0;
        vis_states[0] = 0;
        shmid = shmget(1080 , 8 , 0666 | IPC_CREAT);
        flagPtr = shmat(shmid , 0 , 0);
        flagPtr[0] = false;
        Transition_Function(initial);
    }
}