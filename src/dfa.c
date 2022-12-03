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
#include <assert.h>

int states[500][500];
int n , id;
FILE* output_task2;
bool valid(int x , int y) {
    return x <= n  && y <= n && x >= 0 && y >= 0;
}

void print(int *mask) {
    for(int i = 0 ; i < 50 ; i++)
        printf("%d" , mask[i]);
    printf("\n");
}

// Transition function for symbol 0
int *transition_func_zero(int *mask) {
    static int zero[500];
    memset(zero , 0 , sizeof(zero));
    for(int i = 0 ; i < 500 ; i++) {
        if(mask[i] == 1) {
            int y = i / (n + 1) , x = i - y * (n + 1);
            if(valid(x - 1 , y)) {
                int new_id = i - 1;
                zero[new_id] = 1;
            }
            if(valid(x + 1 , y)) {
                int new_id = i + 1;
                zero[new_id] = 1;
            }
        }
    }
    return zero;
}

// Transition function for symbol 1
int *transition_func_one(int *mask) {
    static int one[500] = {0};
    memset(one , 0 , sizeof(one));
    for(int i = 0 ; i < 500 ; i++) {
        if(mask[i] == 1) {
            int y = i / (n + 1) , x = i - y * (n + 1);
            if(valid(x , y - 1)) {
                int new_id = i - (n + 1);
                one[new_id] = 1;
            }
            if(valid(x , y + 1)) {
                int new_id = i + (n + 1);
                one[new_id] = 1;
            }
        }
    }
    return one;
}

// Checking if mask (array 1) = mask (array 2)
bool cmp(int *first , int *second) {
    for(int i = 0 ; i < 500 ; i++)
        if(first[i] != second[i])
            return false;
    return true;
}


// Recursive NFA to DFA conversion
void getStates(int mask[]) {
    int *one = transition_func_one(mask);
    int *zero = transition_func_zero(mask);
    for(int i = 0 ; i < 500 ; i++)
        states[id][i] = mask[i];
    id++;
    int new_one[500] , new_zero[500];

    for(int i = 0 ; i < 500 ; i++)
        new_one[i] = one[i];
    for(int i = 0 ; i < 500 ; i++)
        new_zero[i] = zero[i]; 

    bool seen_one = false , seen_zero = false;
    for(int i = 0 ; i < id ; i++) {
        if(cmp(states[i] , new_one)) 
            seen_one = true;

        if(cmp(states[i] , new_zero)) 
            seen_zero = true;
    }

    if(!seen_one)
        getStates(new_one);
    if(!seen_zero)
        getStates(new_zero);
} 

//Table Filling Algorithm / DFA minimisation
void minimise_dfa(int M) {
    n = M;
    memset(states , 0 , sizeof(states));
    int start[500] = {0};
    start[0] = 1;
    id = 0;
    int sz = 0;
    getStates(start);

    // Getting unique DFA states
    int unique[(n + 1) * (n + 1) + 5][500];
    memset(unique , 0 , sizeof(unique));
    for(int i = 0 ; i < id ; i++) {
        bool seen = false;
        for(int j = 0 ; j < sz ; j++) {
            if(cmp(unique[j] , states[i]))
                seen = true;
        }
        if(!seen) {
            for(int j = 0 ; j < 500 ; j++)
                unique[sz][j] = states[i][j];
            sz++;
        }
    }

    //Initialising table with (Qa , Qb) such that exactly one of them is a final state
    int table[sz][sz];
    memset(table , 0 , sizeof(table));
    for(int i = 0 ; i < sz ; i++) {
        for(int j = 0 ; j < sz ; j++) {
            int sum = unique[i][(n + 1) * n + n] + unique[j][(n + 1) * n + n];
            if(sum == 1) {
                table[i][j] = 1;
                table[j][i] = 1;
            }
        }
    }

    // Table filling Algorithm
    while(true) {
        int marked = 0;
        for(int i = 0 ; i < sz ; i++) {
            for(int j = 0 ; j < sz ; j++) {
                if(table[i][j]) 
                    continue;
                int *zero_f = transition_func_zero(unique[i]);
                int *zero_s = transition_func_zero(unique[j]);
                int id1 = 0 , id2 = 0;
                for(int i = 0 ; i < sz ; i++) {
                    if(cmp(unique[i] , zero_f)) 
                        id1 = sz;
                    if(cmp(unique[j] , zero_s))
                        id2 = sz;
                }
                if(table[id1][id2]) {
                    marked++;
                    table[i][j] = 1;
                    table[j][i] = 1;
                }
                int *one_f = transition_func_one(unique[i]);
                int *one_s = transition_func_one(unique[j]);
                id1 = 0 , id2 = 0;
                for(int i = 0 ; i < sz ; i++) {
                    if(cmp(unique[i] , one_f)) 
                        id1 = sz;
                    if(cmp(unique[j] , one_s))
                        id2 = sz;
                }
                if(table[id1][id2]) {
                    marked++;
                    table[i][j] = 1;
                    table[j][i] = 1;
                }
            }
        }
        if(!marked)
            break;
    }

    // Checking Equivalent States
    int equivalent_classes[(n + 1) * (n + 1) + 5][500];
    memset(equivalent_classes , 0 , sizeof(equivalent_classes));
    for(int i = 0 ; i < sz ; i++) {
        for(int j = 0 ; j < 500 ; j++)
            equivalent_classes[i][j] = unique[i][j];
        for(int j = 0 ; j < sz ; j++) {
            if(i == j)
                continue;
            if(!table[i][j]) {
                for(int k = 0 ; k < 500 ; k++) {
                    if(unique[j][k])
                        equivalent_classes[i][k] = 1;
                }
            }
        }
    }

    

    // Getting final minimised dfa
    int min_sz = 0;
    int minimised_dfa[(n + 1) * (n + 1) + 5][500];
    memset(minimised_dfa , 0 , sizeof(minimised_dfa));
    for(int i = 0 ; i < sz ; i++) {
        bool seen = false;
        for(int j = 0 ; j < min_sz ; j++) {
            if(cmp(minimised_dfa[j] , equivalent_classes[i]))
                seen = true;
        }
        if(!seen) {
            for(int j = 0 ; j < 500 ; j++)
                minimised_dfa[min_sz][j] = equivalent_classes[i][j];
            min_sz++;
        }
    }

    int min_dfa[(2 * min_sz + 1)][min_sz];

    // Final States
    for(int i = 0 ; i < min_sz ; i++) {
        if(minimised_dfa[i][n * (n + 1) + n]) 
            min_dfa[0][i] = 1;
    }

    // Transition Function for 0
    for(int i = 1 ; i < min_sz + 1 ; i++) {
        int *zero = transition_func_zero(minimised_dfa[i - 1]);
        for(int j = 0 ; j < min_sz ; j++) {
            min_dfa[i][j] = cmp(zero , minimised_dfa[j]);
        }
    }

    // Transition for 1
    for(int i = sz + 1 ; i < 2 * min_sz + 1 ; i++) {
        int *one = transition_func_one(minimised_dfa[i - sz - 1]);
        for(int j = 0 ; j < sz ; j++) {
            min_dfa[i][j] = cmp(one, minimised_dfa[j]);
        }
    }

    output_task2 = fopen("2020A7PS1080H_t2.txt" , "w+");
    for(int i = 0 ; i < 2 * min_sz + 1 ; i++) {
        for(int j = 0 ; j < min_sz ; j++) {
            fprintf(output_task2 , "%d " , min_dfa[i][j]);
        }
        fprintf(output_task2 , "\n");
    }
    fclose(output_task2);
}
