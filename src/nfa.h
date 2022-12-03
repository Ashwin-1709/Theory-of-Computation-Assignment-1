#ifndef NFA_H // Edit TEMPLATE to be the name of the file, e.g: if your file is `src/nfa.h` replace TEMPLATE_H with NFA_H
#define NFA_H // Make the same changes here.

// All struct definitions and function declarations go here.
// You can optionally add function definitions here as well but it is recommended
// that they be added to the corresponding .c file.

struct state {
    int x , y;
};

bool isValidState(struct state cur_state);
void Transition_Function(struct state cur);
void startNFA(int sz , char ip[] , int len);

#endif
