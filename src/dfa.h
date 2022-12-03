#ifndef DFA_H
#define DFA_H

bool valid(int x , int y);
int *transition_func_zero(int *mask);
int *transition_func_one(int *mask);
bool cmp(int *first , int *second);
void getStates(int mask[]);
void minimise_dfa();

#endif