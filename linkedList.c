#include "linkedList.h"

struct listaLigada* criaLista ()
{
    struct listaLigada *lista;

    lista = (struct listaLigada*)malloc (sizeof (struct listaLigada));
    if (!lista) return NULL;

    lista->tam = 0;
    return lista;
}

char insereLista (struct nodo *novoN, struct listaLigada *l)
{
    if (!novoN || !l) return 0;
    
    if (!l->tam) l->ini = novoN;
    else l->fim->prox = novoN;
    l->fim = novoN;
    ++l->tam;

    return 1;
}

struct nodo* criaNodoStr (char *str, char status)
{
    struct nodo *novoN;
    char *ptrTemp;

    novoN = (struct nodo*)malloc (sizeof (struct nodo));
    if (!novoN) return NULL;

    ptrTemp = strdup (str);
    if (!ptrTemp)
    {
        free (novoN);
        return NULL;
    }

    novoN->status = status;
    novoN->ptr = (void*)ptrTemp;
    novoN->prox = NULL;

    return novoN;
}

struct nodo* criaNodoInt (int num)
{
    struct nodo *novoN;
    int *ptrTemp;

    novoN = (struct nodo*)malloc (sizeof (struct nodo));
    if (!novoN) return NULL;

    ptrTemp = (int*)malloc (sizeof (int));
    if (!ptrTemp)
    {
        free (novoN);
        return NULL;
    }
    
    *ptrTemp = num;
    novoN->ptr = (void*)ptrTemp;
    novoN->prox = NULL;

    return novoN;
}

void modificaNodoStr (struct nodo *n, char status)
{
    if (!n) return;

    n->status = status;
}

char removeListaL (struct listaLigada* l, void **ptr, char *status)
{
    void *ptrAux;
    struct nodo *nodoAux;
    if (!l || !l->tam) return 0; 

    nodoAux = l->ini;
    l->ini = l->ini->prox;
    if (l->tam == 1) l->fim = NULL;

    *ptr = nodoAux->ptr;
    *status = nodoAux->status;
    free (nodoAux);
    --l->tam;
}