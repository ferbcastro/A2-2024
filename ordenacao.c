#include "ordenacao.h"

#define GE(str1, str2) (strcmp (str1, str2) >= 0)
#define L(str1, str2) (strcmp (str1, str2) < 0)

char maiorIgual (char *str1, char *str2)
{

}

void mergeStr (char **vetor, long meio, long tam)
{
    char **vetTemp;
    int a, b, i;

    vetTemp = (char**)malloc (tam * sizeof(char*));
    if (!vetTemp)
    {
        printf ("Erro de alocacao\n");
        return;
    }
    
    /* interpola os subvetores ordenados em vetTemp */
    a = i = 0;
    b = meio;
    while (a < meio && b < tam)
    {
        if (GE(vetor[b], vetor[a]))
        {
            vetTemp[i] = vetor[a];
            a++;
        }
        else
        {
            vetTemp[i] = vetor[b];
            b++;
        }
        i++;
    }

    /* copia o resto dos elementos do subvetor 
     * que nao foi todo percorrido */
    if (a >= meio)
        for (; b < tam; b++, i++) vetTemp[i] = vetor[b];
    else
        for (; a < meio; a++, i++) vetTemp[i] = vetor[a];
    /* transfere os elementos ordenados de 
     * vetTemp para vetor */
    for (i = 0; i < tam; i++) vetor[i] = vetTemp[i];

    free (vetTemp);
}

void mergeSort (char **vetor, long tam)
{
    int meio = tam / 2;

    if (tam > 1)
    {
        mergeSortStr (vetor, meio);
        mergeSortStr (vetor + meio, tam - meio);
        mergeStr (vetor, meio, tam);
    }

    return;
}