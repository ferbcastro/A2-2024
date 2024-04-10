#ifndef __CSV__
#define __CSV__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define LINS_FIX 10
#define TAM_MAX 1024
#define TAM_PONTOS 3
#define TAM_LINS_FIX 1

typedef char (*operacao)(char *, char *);

typedef struct linha
{
    char *str;
    char completa;
} linha;

typedef struct descricao
{
    char *moda;
    char **col;
    long modaQtd;
    long cont;
    long *unicos;
    long qtdUnicos;
    double media;
    double mediana;
    double desvioP;
    double min;
    double max;
} descricao;

typedef struct csvFile
{
    FILE *arq;
    linha *vetLinhas;

    char *tipos;
    short int maxTamNro;
    int cols;
    int *vetF;
    int *selecionaCols;
    int tamSelecionaCols;
    long *ordemLinhas;
    long linsF[LINS_FIX];
    long lins;
} csvFile;

char *formataL (csvFile *arqCSV, char *str, char *linha, long lin);
char detectaTipoStr (char *str);

csvFile *abreCSV (char *pathArquivoCSV);
void *fechaCSV (csvFile *arqCSV);
void mostraCSV (csvFile *arqCSV);
void sumarioCSV (csvFile *arqCSV);
void filtraCSV (csvFile *arqCSV, char (*filtro)(char *, char *));
void ordenaCSV (csvFile *arqCSV);
void descricaoCSV (csvFile *arqCSV);
void selecionaCSV (csvFile *arqCSV);
void zerosCSV (csvFile *arqCSV);

#endif