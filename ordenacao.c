#include "ordenacao.h"

char maiorIgual (char *str1, char *str2)
{   
    double res;

    if (!atoi (str1) || !atoi (str2))
        res = (double)strcmp (str1, str2);
    else 
        res = atof (str1) - atof (str2);

    return (res >= 0);
}

void mergeStr (char **vet, int ini, int fim, long *pos)
{
    long *posTemp;
    int i, meio, tam, a, b;
    
    tam = fim - ini + 1;

    posTemp = (long*)malloc (tam * sizeof(long));
    if (!posTemp)
    {
        printf ("Erro de alocacao\n");
        return;
    }
    
    // interpola os subvetores ordenados 
    i = 0;
    a = ini;
    meio = (ini + fim) / 2;
    b = meio + 1;
    while (a <= meio && b <= fim)
    {
        if (maiorIgual (vet[pos[b]], vet[pos[a]]))
        {
            posTemp[i] = pos[a];
            a++;
        }
        else
        {
            posTemp[i] = pos[b];
            b++;
        }
        i++;
    }

    // copia o resto dos elementos do subvetor 
    // que nao foi todo percorrido 
    if (a > meio)
        for (; b <= fim; b++, i++) posTemp[i] = pos[b];
    else
        for (; a <= meio; a++, i++) posTemp[i] = pos[a];
    
    // transfere os elementos ordenados de 
    // vetTemp para vetor 
    for (i = 0; i < tam; i++, ini++) pos[ini] = posTemp[i];

    free (posTemp);
}

void mergeSort (char **vetor, int ini, int fim, long *pos)
{
    int meio = (ini + fim) / 2;

    if (ini < fim)
    {
        mergeSort (vetor, ini, meio, pos);
        mergeSort (vetor, meio + 1, fim, pos);
        mergeStr (vetor, ini, fim, pos);
    }

    return;
}