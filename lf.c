#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#define epsilon '#'
#define MAX_FILE_LEN 255
#include "functions.h"
//dichiarazione di funzioni e variabili globali
void parse();
struct symbol** first(struct symbol* s);
void calculatefollow();
void calculatefirst();
void reconstruct();
struct symbol** follow(struct symbol* s);
struct symbol** sfirst(struct symbol** body,int k,int size);
struct record** LLtable();
void tdparsing(struct record** table);


FILE* ptr;
FILE* fp; 
struct symbol** nonterminals;
struct symbol** terminals;
struct symbol** nt;
struct symbol** t;
struct production* grammar;
struct symbol* eps;
struct symbol* dollar;
struct symbol** word;
struct stack* input;
struct stack* ntstack;
int n_index;
int t_index;
int g_index;
char* _word;
int w_len;
struct record* error;


void printgrammar(){ //funzione per stampare tutte le produzioni della grammatica inserita
    for(int i=0; i<g_index; i++){
            fprintf(ptr,"Produzione: %c->",grammar[i].sym->letter);
            for(int k=0; k<grammar[i].size;k++){
                fprintf(ptr,"%c",grammar[i].body[k]->letter);
            }
            fprintf(ptr,"\n");
            
     }
}
int findpos(struct symbol* s,struct symbol** set,int size){ //funzione per trovare la poszione di un simbolo all'interno dei set (dinamici)
    for(int i=0;i<size;i++){
        if(set[i]->letter==s->letter){
            return i;
        }
    }
    return -1;
}
void printprod(struct production* p){ //funzione per stampare una singola produzione
    fprintf(ptr,"%c->",p->sym->letter);
    for(int i=0; i<p->size;i++){
        fprintf(ptr,"%c",p->body[i]->letter);
    }
    fprintf(ptr,"\n");
}
void printtable(struct record** table, int row, int col){ //funzione per stampare le celle della tabella di parsing
    fprintf(ptr,"\n--- Tabella di Parsing ---\n");
    for(int i=0; i<row;i++){
        for(int j=0;j<col;j++){
            if(table[(i*col)+j]!=error){
                fprintf(ptr,"M[%c,%c]=",table[(i*col)+j]->nonterminal->letter,table[(i*col)+j]->terminal->letter);
                printprod(table[(i*col)+j]->prod);
                
            }
        }
    }
}

int main(int argc,char*argv[]){
    //inizializzazione delle variabili globali
    char in[MAX_FILE_LEN];
    char out[MAX_FILE_LEN];
    int flagi = 0;
    int flago = 0;
    char opt;
    while((opt = getopt(argc, argv, "i:o:")) != -1) 
    { 
        switch(opt) 
        { 
            
            case 'i': 
                flagi = 1;
                printf("Using custom input %s\n",optarg);
                sprintf(in,"inputs/%s",optarg);
                break; 
            case 'o': 
                flago= 1;
                printf("Using custom output %s\n",optarg);
                sprintf(out,"outputs/%s",optarg);
                break;
            default:
                printf("Usage ./PredictiveParsing [-i FILENAME][-o FILENAME]");
                exit(-1);
        } 
    }
    if(flagi==0){
        printf("Using default input input1.txt\n");
        strcpy(in,"inputs/input1.txt");
    }
    if(flago==0){
        printf("Using default output output1.txt\n");
        strcpy(out,"outputs/output1.txt");
    }
    
    fp = fopen(in,"r");
    ptr = fopen(out,"w");
    fprintf(ptr,"/* Output del file %s */\n\n",in);
    ntstack = createstack(100);
    eps = (struct symbol*) malloc (sizeof(struct symbol));
    dollar = (struct symbol*) malloc (sizeof(struct symbol));
    eps->letter = epsilon;
    dollar->letter = '$';
    nonterminals = createset();
    terminals = createset();
    //add(dollar,terminals);
    n_index = 0;
    t_index = 0;
    g_index = 0;
    error = (struct record*) malloc (sizeof(struct record));
    parse();
    fprintf(ptr,"--- Grammatica di partenza --- \n");
    printgrammar();
    word = (struct symbol**) malloc (sizeof(struct symbol*)*w_len+1);
    for(int i=0; i<w_len;i++){
        word[i] = findchar(_word[i],terminals);
        if(word[i]==NULL){
            fprintf(ptr,"Il terminale %c non appartiene alla grammatica, rieseguire il programma con un input corretto",_word[i]);
            
            exit(-1);
        }
        
    }
    
    word[w_len] = dollar;
    w_len++;
    input = createstack(w_len);
    for(int i=w_len-1; i>=0;i--){
        addstack(input,word[i]);
    }
    nt = (struct symbol**) malloc (n_index*sizeof(struct symbol*));
    t = (struct symbol**) malloc (t_index*sizeof(struct symbol*));
    fprintf(ptr,"\n--- Parola da verificare ---\n");
    printstack(input,ptr);
    addstack(ntstack,dollar);
    addstack(ntstack,grammar[0].sym);
   
    

    

    int counttmp=0;
    for(int i = 0; i<93;i++){
        if(nonterminals[i]!=NULL){ 
            nt[counttmp] = nonterminals[i];
            counttmp++;
        }
    }
    counttmp=0;
    for(int i = 0; i<93;i++){
        if(terminals[i]!=NULL && terminals[i]->letter!=epsilon){
            t[counttmp] = terminals[i];
            counttmp++;
        }
    }
    t[counttmp] = dollar;
    calculatefirst();
    calculatefollow(dollar);
    struct record** table = LLtable();
    printtable(table,n_index,t_index);
    tdparsing(table);

    //free
    fclose(fp);
    fclose(ptr);
    free(nonterminals);
    free(terminals);
    free(nt);
    free(t);
    free(grammar);
    free(eps);
    free(dollar);
    free(word);
    free(input);
    free(ntstack);
    free(_word);
    free(error);

}

void parse(){ //funzione per fare parsing del file di input
    int N;
    fscanf(fp,"%d",&N);
    grammar = (struct production*) malloc (N*sizeof(struct production));
    char buffer[255];
    fscanf(fp,"%s",buffer);
    w_len = strlen(buffer);
    
    _word = (char*) malloc (sizeof(char)*w_len);
    strcpy(_word,buffer);

    while(fgets(buffer,sizeof(buffer),fp)!=NULL){
        buffer[strcspn(buffer,"\n")] = 0;
        struct symbol* s = (struct symbol*) malloc (sizeof(struct symbol));
        s->letter = buffer[0];
        if(findchar(s->letter,nonterminals)!=NULL){
            s = findchar(s->letter,nonterminals);
        }else{
            
            s->terminal = 0;
            s->firsts = createset();
            s->follow = createset();
            s->tofollow = createset();
        }
        char pds[255];
        strncpy(pds,buffer+2,strlen(buffer));
        int count = 0;
        char delim[] = "|";
        char* ptr = strtok(pds,delim);
        if(findchar(s->letter,nonterminals)==NULL){
            add(s,nonterminals);
            n_index++;
        }
        while(ptr!=NULL){
            struct production tmp;
            tmp.body = (struct symbol**) malloc (strlen(ptr)*sizeof(struct symbol*));
            int i=0;
            tmp.size = 0;
            tmp.sym = s;
            for(int k=0; k<strlen(ptr);k++){
                if(findchar(ptr[k],terminals)!=NULL){
                    tmp.body[i]=findchar(ptr[k],terminals);
                    tmp.size++;
                    i++;
                }else{
                    if(findchar(ptr[k],nonterminals)!=NULL){
                        tmp.body[i]=findchar(ptr[k],nonterminals);
                        tmp.size++;
                        i++;
                    }else{
                        struct symbol* kk = (struct symbol*) malloc (sizeof(struct symbol));
                       
                        kk->letter = ptr[k];
                        kk->firsts = createset();
                        kk->follow = createset();
                        kk->tofollow = createset();
                        
                        if(ptr[k]>=65 && ptr[k]<=90){
                            kk->terminal = 0;
                            n_index++;
                            add(kk,nonterminals);
                        }else{
                            kk->terminal = 1;
                            t_index++;
                            add(kk,kk->firsts);     
                            add(kk,terminals);
                        }
                        tmp.body[i]=kk;
                        tmp.size++;
                        i++;

                    }
                }
            }
            grammar[g_index]=tmp;
            g_index++;
            ptr = strtok(NULL,delim);
        }
    }
}
void calculatefirst(){ //funzione wrapper per il calcolo dei first
    fprintf(ptr,"\n--- Calcolo First ---\n");
    for(int i=93;i>=0;i--){
        if(nonterminals[i]!=NULL){
            nonterminals[i]->firsts = first(nonterminals[i]);
            fprintf(ptr,"Firsts %c ",nonterminals[i]->letter);
            printset(nonterminals[i]->firsts,ptr);
            fprintf(ptr,"\n");
        }
    }
}
struct symbol** first(struct symbol* s){ //funzione per l'effettivo calcolo dei first
    struct symbol** firsts = createset();
    for(int i=0; i<g_index;i++){
        if(grammar[i].sym->letter == s->letter){
            for(int k=0;k<grammar[i].size;k++){
                if(grammar[i].body[k]->terminal){
                    add(grammar[i].body[k],firsts);
                    break;
                }else{
                    if(grammar[i].body[k]->firsts!=NULL){
                        if(grammar[i].body[k]->letter != s->letter){
                            firsts = merge(grammar[i].body[k]->firsts,firsts);
                        }
                        
                    }
                    firsts = merge(firsts,first(grammar[i].body[k]));
                    if(!findchar(epsilon,firsts)){
                        break;
                    }
                }
            }
        }
    }
    return firsts;
}
void calculatefollow(){ //funzione wrapper per il calcolo dei follow
    add(dollar,grammar[0].sym->follow);
    for(int i = 0; i<93; i++){
        if(nonterminals[i]!=NULL){
            nonterminals[i]->follow = merge(nonterminals[i]->follow,follow(nonterminals[i]));
            removechar(nonterminals[i]->letter,nonterminals[i]->tofollow);
        }   
    }
    fprintf(ptr,"\n--- Calcolo Follow ---\n");
    for(int i = 0; i<93; i++){
        if(nonterminals[i]!=NULL){
            reconstruct();
            fprintf(ptr,"Follow %c",nonterminals[i]->letter);
            printset(nonterminals[i]->follow,ptr);
            fprintf(ptr,"\n");
        }
    }
}
struct symbol** follow(struct symbol* s){ //funzione effettiva per il calcolo dei follow
    struct symbol** follows = createset();
    for(int i=0; i<g_index; i++){
        for(int k=0; k<grammar[i].size;k++){
            if(s == grammar[i].body[k]){
                struct symbol** tmp = createset();
                if(k<grammar[i].size-1){
                    tmp = sfirst(grammar[i].body,k+1,grammar[i].size);
                    follows = merge(follows,tmp);
                    removechar(epsilon,follows);       
                }
                if(k==grammar[i].size-1 || findchar(epsilon,tmp)!=NULL){
                    add(grammar[i].sym,s->tofollow);           
                }
                break;
            }
        }
    }
    
    return follows;
}
struct symbol** sfirst(struct symbol** body,int k,int size){ //funzione per il calcolo dei first del body di una produzione
    int flag = 0;
    struct symbol** firsts = createset();
    for(int i = k; i<size;i++){
        struct symbol** tmp = createset();
        copy(body[i]->firsts,tmp);
        //removechar(epsilon,tmp);
        firsts = merge(firsts,tmp);
        if(findchar(epsilon,body[i]->firsts)==NULL){
            flag = 1;
            break;
            
        }
    }
    if(flag == 0){
        add(eps,firsts);
        
    }   
    return firsts;
}
void reconstruct(){ //funzione per risolvere le dipendenze dei follow
        for(int i = 0; i< 93; i++){
            if(nonterminals[i]!=NULL){
                for(int j=0; j<93; j++){
                    for(int k=0; k<93; k++){
                        if(nonterminals[i]->tofollow[k]!=NULL){
                        nonterminals[i]->follow = merge(nonterminals[i]->tofollow[k]->follow,nonterminals[i]->follow);
                        }
                    }
                    
                }
                
               
            }
        }
}
int calculateindex(struct symbol* s1,struct symbol* s2){ //funzione per il calcolo per la posizione nella tabella di parsing
    int x = findpos(s1,nt,n_index);
    int y = findpos(s2,t,t_index);
    int ris = (x*(t_index+1))+y;
    return ris;
}
struct record** LLtable(){ //funzione per la creazione della tabella di parsing
    int row = n_index;
    int col = t_index;
    
    struct record** table  = (struct record**)malloc ((row)*(col)*sizeof(struct record*));
    for(int i=0; i<row*col; i++){
        table[i] = error;
    }
    
    struct symbol** firsts;
    struct symbol** follows;
    for(int i=0; i<g_index;i++){
        firsts = sfirst(grammar[i].body,0,grammar[i].size);
        follows = grammar[i].sym->follow;
        
        for(int j=0;j<93;j++){
            if(firsts[j]!=NULL && firsts[j]->letter!=epsilon){
                
                struct record* tmp = (struct record*) malloc(sizeof(struct record));
                tmp->nonterminal = grammar[i].sym;
                tmp->terminal = firsts[j];
                tmp->prod = &grammar[i];
                int x = findpos(firsts[j],t,t_index);
                int y = findpos(grammar[i].sym,nt,n_index);
                if(table[calculateindex(grammar[i].sym,firsts[j])]!=error){fprintf(ptr,"\n--- Risultato --- \nIl linguaggio non è LL(1), conflitto nella tabella di parsing");exit(-1);}
                table[calculateindex(grammar[i].sym,firsts[j])] = tmp;
            }
        }
        if(findchar(epsilon,firsts)!=NULL){
            
           for(int j=0;j<93;j++){
                if(follows[j]!=NULL){
                    struct record* tmp = (struct record*) malloc(sizeof(struct record));
                    tmp->nonterminal = grammar[i].sym;
                    tmp->terminal = follows[j];
                    tmp->prod = &grammar[i];
                    int x = findpos(follows[j],t,t_index);
                    
                    int y = findpos(grammar[i].sym,nt,n_index);
                    if(table[calculateindex(grammar[i].sym,follows[j])]!=error){fprintf(ptr,"\n--- Risultato --- \nIl linguaggio non è LL(1), conflitto nella tabella di parsing");exit(-1);}
                    table[calculateindex(grammar[i].sym,follows[j])] = tmp;
                   
                }
            }
            
        }

    }
   return table;
}

void tdparsing(struct record** table){ //funzione per lo svolgimento dell'algoritmo di parsing predittivo
    fprintf(ptr,"\n--- Passi di Parsing ---\n");
    int count = 1;
    struct symbol* b = top(input);
    struct symbol* X = top(ntstack);
    while(X!=dollar){
        fprintf(ptr,"%d:\n",count);
        fprintf(ptr,"Nonterminal stack: ");
        printstack(ntstack,ptr);
        fprintf(ptr,"Input stack: ");
        printstack(input,ptr);
        if(X->letter == '#'){
            pop(ntstack);
            X = top(ntstack);
            fprintf(ptr,"Esegui pop della testa dei non terminali: epsilon trovato\n");
        }
        else if(X==b){
            pop(ntstack);
            pop(input);
            b = top(input);
            fprintf(ptr,"Match tra stack dei terminali e non terminali. Esegui il pop di entrambi\n");
        }
        else if(X->terminal){fprintf(ptr,"--- Risultato --- \nERRORE, la parola non appartiene al linguaggio"); return;}
        else if(table[calculateindex(X,b)]==error){fprintf(ptr,"--- Risultato --- \nERRORE, la parola non appartiene al linguaggio");return;}
        else{
            fprintf(ptr,"Esegui la produzione: ");
            printprod(table[calculateindex(X,b)]->prod);
            
            pop(ntstack);
            for(int i = table[calculateindex(X,b)]->prod->size-1; i>=0; i--){
                addstack(ntstack,table[calculateindex(X,b)]->prod->body[i]);
            }
        }
        count++;
        fprintf(ptr,"\n");
        X = top(ntstack);
        
    }

    fprintf(ptr,"--- Risultato --- \nACCEPT");

}