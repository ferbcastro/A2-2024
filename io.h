#ifndef __CSV__
#define __CSV__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define LINS_FIX 10
#define TAM_MAX 1024
#define TAM_PONTOS 3

typedef struct linha
{
    char *str;
    char completa;
} linha;

typedef struct csvFile
{
    FILE *arq;
    linha *vetLinhas;
    char *tipos;
    char *selecionaCols;
    short int *vetF;
    short int maxTamNro;
    long linsF[LINS_FIX];
    long lins;
    int cols;
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