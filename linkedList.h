#ifndef __LIST__
#define __LIST__

#include <stdlib.h>
#include <string.h>

struct nodo
{
    char status;
    void *ptr;
    struct nodo *prox;
};

struct listaLigada
{
    struct nodo *ini;
    struct nodo *fim;
    long tam;
};

struct listaLigada* criaLista ();
char insereLista (struct nodo *novoN, struct listaLigada *l);
struct nodo* criaNodoStr (char *str, char status);
struct nodo* criaNodoInt (int num);
void modificaNodoStr (struct nodo *n, char status);
char removeListaL (struct listaLigada* l, void **ptr, char *status);
#endif