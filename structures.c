#include "functions.h"

struct stack* createstack(int dim){
    struct stack* tmp = (struct stack*) malloc (sizeof(struct stack));
    tmp->head = 0;
    tmp->pila = (struct symbol**) malloc (dim*sizeof(struct symbol*));
    tmp->size = dim;
    return tmp; 
}
struct symbol* top(struct stack* s){
    return s->pila[s->head-1];
}
struct symbol* pop(struct stack* s){

    s->head = s->head-1;
    return  s->pila[s->head];
}
int isFull(struct stack* s){
    if(s->head == s->size){
        return 1;
    }
    return 0;
}
int isEmpty(struct stack* s){
    if(s->head==0){
        return 1;
    }
    return 0;
}
void addstack(struct stack* s, struct symbol* p){
    if(!isFull(s)){

           //printf("s->head = %d",s->head);
           int i = s->head;
           s->pila[s->head] = p;
           i++;
           s->head = i;
          //printf("Ho aggiunto il simbolo %c in posizione %d",s->pila[s->head-1]->letter,s->head);
    }
}
void printstack(struct stack* s,FILE* ptr){
    fprintf(ptr,"[");
    for(int i=s->head-1;i>=0;i--){
        fprintf(ptr," %c ",s->pila[i]->letter);
    }
    fprintf(ptr,"]\n");
}

struct symbol** createset(){
    
    struct symbol** tmp =  (struct symbol**) malloc (93*sizeof(struct symbol*));
    return tmp;
}

int add(struct symbol* s,struct symbol** set){
    char c = s->letter;
    int i = c-33;
    if(set[i]!=NULL){
        //printf("Non aggiunto: carattere %c già presente\n",c);
        return -1;}
    set[i]=s;

    //printf("Char %c aggiunto all'index %i\n",set[i]->letter,i);
    return 1;

}

struct symbol* findchar(char c,struct symbol** s){
     if(s[c-33]!=NULL){
         return s[c-33];
     }
     return NULL;
}

struct symbol** merge(struct symbol** s1,struct symbol** s2){
    struct symbol** dest = createset();
    //struct symbol** tmp = (struct symbol**) malloc (93*sizeof(struct symbol*));
    for(int i=0; i<93;i++){
        
        if(s1[i]!=NULL){
            dest[i]=s1[i];
        }else{
            if(s2[i]!=NULL){
                dest[i]=s2[i];
            }
        }
        
    }
    return dest;

}
void removechar(char c, struct symbol** set){
    if(set[c-33]!=NULL){
        set[c-33]=NULL;
    }
}
void printset(struct symbol** s,FILE* ptr){
    fprintf(ptr,"[");
    for(int i = 0; i<93;i++){
        if(s[i]!=NULL){
            fprintf(ptr,"%c ,",s[i]->letter);
        }
    }
    fprintf(ptr,"]");
}
int equals(struct symbol** s1, struct symbol** s2){
    
    int flag = 1;
    for(int i = 0; i<93; i++){
            if(s1[i]!=s2[i]){
                    flag =  0;
                }
    }
                
    return flag;
}

void copy(struct symbol** source, struct symbol** dest){
    for(int i =0; i<93;i++){
        dest[i]=source[i];
    }
}