#ifndef HASHSET
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct symbol{
    char letter;
    int terminal;
    struct symbol** firsts;
    struct symbol** follow;
    struct symbol** tofollow;
    
};
struct production{
    struct symbol* sym;
    struct symbol** body;
    int size;
};
struct record{
    struct symbol* terminal;
    struct symbol* nonterminal;
    struct production* prod;
};

struct stack{
    int size;
    int head;
    struct symbol** pila;
};
struct stack* createstack(int dim);
struct symbol* top(struct stack* s);
struct symbol* pop(struct stack* s);
int isFull(struct stack* s);
int isEmpty(struct stack* s);
void addstack(struct stack* s, struct symbol* p);
void printstack(struct stack* s, FILE* ptr);

struct symbol** createset();

int add(struct symbol* s,struct symbol** set);

struct symbol* findchar(char c,struct symbol** s);

struct symbol** merge(struct symbol** s1,struct symbol** s2);
void removechar(char c, struct symbol** set);
void printset(struct symbol** s,FILE* ptr);
int equals(struct symbol** s1, struct symbol** s2);

void copy(struct symbol** source, struct symbol** dest);
#endif