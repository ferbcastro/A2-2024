#include "io.h"
#include "linkedList.h"
#include "ordenacao.h"

// inicio funcoes auxiliares globais

void msgFalhaAlocacao ()
{
    printf ("Erro na alocacao de memoria\n");
}

short int tamanhoNro (long numero)
{
    int i = 1;

    while (numero /= 10) i++;

    return i;
}

char detectaTipo (char *str)
{
    if (*str == '\0' || *str == '0' || atoi (str)) return 'N';

    return 'S';
}

void obtemLinhaUser (char *buffer)
{
    scanf ("%1024[^\n]", buffer);
    getchar ();    
}

char obtemLinhaArq (FILE *arquivo, char *buffer)
{
    int ret;
    if (!arquivo || !buffer) return 0;

    ret = fscanf (arquivo, "%1024[^\n]", buffer);
    if (ret == EOF) return 0;
    fseek (arquivo, 1, SEEK_CUR);

    return 1;
}

char* iniciaSeparadorStr (char *ptr, int sep, long tamStr)
{
    char *posSep;

    if (!ptr || tamStr < 0) return NULL;
    if (tamStr == 0 && *(ptr - 1) != '\0') return NULL;

    posSep = strchr (ptr, sep);
    if (posSep) *posSep = '\0';

    return ptr;
}

char *separaStr (char *ptr, int sep, long *tam)
{
    long len;

    if (!ptr) return NULL;

    len = strlen (ptr);
    ptr = ptr + len + 1;
    (*tam) = (*tam) - len - 1;

    return iniciaSeparadorStr (ptr, sep, *tam);
}

int indiceColuna (csvFile *arqCSV, char *campo)
{
    char buffer[TAM_MAX + 1];
    char *ptr, sep = ',', achou = 0; 
    long tam;
    int col = 0;

    if (!arqCSV) return -1;

    strcpy (buffer, arqCSV->vetLinhas[0].str);
    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);
    while (ptr && !achou)
    {
        if (!strcmp (campo, ptr)) achou = 1;
        ptr = separaStr (ptr, sep, &tam);
        col++;
    }
    if (!achou) return -1;

    return --col;
}

char *obtemColuna (csvFile* arqCSV, char *buffer, int col)
{
    int i = 0;
    long tam;
    char *ptr, sep = ',';

    if (!arqCSV || col < 0 || col > arqCSV->cols) 
        return NULL;

    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);
    for (i = 0; i < col; i++)
        ptr = separaStr (ptr, sep, &tam);

    return ptr;
}

void salvaEmArquivo (csvFile *arqCSV)
{
    FILE *arq;
    char buffer[TAM_MAX + 1];
    char sep = ',', *ptr, **cols;
    long i, k, tam, j = 0;

    printf ("Deseja gravar um arquivo com ");
    printf ("as variáveis selecionadas? [S|N] ");
    obtemLinhaUser (buffer);

    cols = (char**)malloc (arqCSV->cols * sizeof (char*));
    if (!cols) return;

    if (strlen(buffer) == 1 && buffer[0] == 'S')
    {
        printf ("Entre com o nome do arquivo: ");
        obtemLinhaUser (buffer);

        arq = fopen (buffer, "w");
        if (!arq) return;

        for (i = 0; i < (arqCSV->lins - 1); i++)
        {
            strcpy (buffer, arqCSV->vetLinhas[arqCSV->ordemLinhas[i]].str);
            tam = strlen(buffer);
            ptr = iniciaSeparadorStr (buffer, sep, tam);
            while (ptr)
            {
                cols[j] = strdup(ptr);
                ptr = separaStr (ptr, sep, &tam);
                j++;
            }

            for (j = 0; j < (arqCSV->tamSelecionaCols - 1); j++)
            {
                ptr = cols[arqCSV->selecionaCols[j]];
                fprintf (arq, "%s,", ptr);
            }
            ptr = cols[arqCSV->selecionaCols[j]];
            fprintf (arq, "%s\n", ptr);
        }

        strcpy (buffer, arqCSV->vetLinhas[i].str);
        tam = strlen(buffer);
        ptr = iniciaSeparadorStr (buffer, sep, tam);
        while (ptr)
        {
            cols[j] = strdup(ptr);
            ptr = separaStr (ptr, sep, &tam);
            j++;
        }

        for (j = 0; j < (arqCSV->tamSelecionaCols - 1); j++)
        {
            ptr = cols[arqCSV->selecionaCols[j]];
            fprintf (arq, "%s,", ptr);
        }
        ptr = cols[arqCSV->selecionaCols[j]];
        fprintf (arq, "%s", ptr);

        fclose (arq);
    }
}

// fim funcoes auxiliares globais

// inicio funcoes de abertura e 
// fechamento/auxiliares de csvFile

void copiaListaLemVet (struct listaLigada *l, linha *vet)
{
    void *ptr;
    char c;
    long i = 0;

    while (removeListaL (l, &ptr, &c))
    {
        vet[i].str = (char*)ptr;
        vet[i].completa = c;
        i++;
    }     
}

char verificaCabecalho (csvFile* arqCSV, char *buffer)
{
    char erro = 0;
    long tam, len;
    char *ptr;
    char sep = ',';

    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);
    while (ptr && !erro) 
    {
        ++arqCSV->cols;
        if (!(len = strlen (ptr))) erro = 1;
        ptr = separaStr (ptr, sep, &tam);
    }

    return !erro;
}

csvFile* alocaCSV ()
{
    csvFile *arqCSV;
    arqCSV = (csvFile*)malloc (sizeof (csvFile));
    if (!arqCSV)
    {
        printf ("Erro de alocacao\n");
        exit (1);
    }

    arqCSV->vetLinhas = NULL;
    arqCSV->selecionaCols = NULL;
    arqCSV->ordemLinhas = NULL;
    arqCSV->tipos = NULL;
    arqCSV->vetF = NULL;
    arqCSV->lins = 0;
    arqCSV->cols = 0;

    return arqCSV;
}

csvFile *abreCSV (char *pathArquivoCSV)
{
    csvFile *arqCSV;
    struct listaLigada *lTemp;
    struct nodo *nodoStr;
    char buffer[TAM_MAX + 1], *ptr;
    char s, erro, sep = ',';
    long tam, i, j, len;
    
    arqCSV = alocaCSV ();
    arqCSV->arq = fopen (pathArquivoCSV, "r");
    lTemp = criaLista ();
    if (!arqCSV->arq || !lTemp) 
    {
        msgFalhaAlocacao ();
        free (lTemp);
        return fechaCSV (arqCSV);
    }

    if (!obtemLinhaArq (arqCSV->arq, buffer)) 
        return fechaCSV (arqCSV);
    insereLista (criaNodoStr (buffer, 1), lTemp);
    if (!verificaCabecalho (arqCSV, buffer)) 
        return fechaCSV (arqCSV);
    ++arqCSV->lins;

    arqCSV->tipos = (char*)malloc (arqCSV->cols * sizeof (char));   
    arqCSV->vetF = (int*)malloc (arqCSV->cols * sizeof (int));
    arqCSV->selecionaCols = (int*)malloc (arqCSV->cols * sizeof (int));
    if (!arqCSV->tipos || !arqCSV->vetF || !arqCSV->selecionaCols) 
    {
        msgFalhaAlocacao ();
        free (lTemp);
        return fechaCSV (arqCSV);
    }

    for (j = 0; j < arqCSV->cols; j++) 
        arqCSV->tipos[j] = 'N';

    erro = 0;
    while (obtemLinhaArq (arqCSV->arq, buffer) && !erro)
    {
        ++arqCSV->lins;
        j = 0;

        nodoStr = criaNodoStr (buffer, 1);
        if (!insereLista (nodoStr, lTemp)) erro = 1;

        tam = strlen (buffer);
        ptr = iniciaSeparadorStr (buffer, sep, tam);
        while (ptr && j < arqCSV->cols)
        {
            if (detectaTipo (ptr) == 'S') arqCSV->tipos[j] = 'S';
            if (!(len = strlen (ptr))) modificaNodoStr (nodoStr, 0);
            ptr = separaStr (ptr, sep, &tam);
            j++;
        }

        if (ptr || j != arqCSV->cols) erro = 1;
    }
    if (erro || arqCSV->lins == 1) 
    {
        free (lTemp);  
        return fechaCSV (arqCSV);
    }

    arqCSV->vetLinhas = (linha*)malloc (arqCSV->lins * sizeof (linha));
    arqCSV->ordemLinhas = (long*)malloc (arqCSV->lins * sizeof (long));
    if (!arqCSV->vetLinhas || !arqCSV->ordemLinhas) 
    {
        msgFalhaAlocacao ();
        free (lTemp);
        return fechaCSV (arqCSV);
    }
    
    copiaListaLemVet (lTemp, arqCSV->vetLinhas);

    free (lTemp);
    return arqCSV;
}

void *fechaCSV (csvFile *arqCSV)
{
    if (!arqCSV) return NULL;
    if (arqCSV->arq) fclose (arqCSV->arq);
    if (arqCSV->vetF) free (arqCSV->vetF);
    if (arqCSV->tipos) free (arqCSV->tipos);
    if (arqCSV->selecionaCols) free (arqCSV->selecionaCols);
    if (arqCSV->ordemLinhas) free (arqCSV->ordemLinhas);
    if (arqCSV->vetLinhas)
    {
        for (int i = 0; i < arqCSV->lins; i++) 
            free (arqCSV->vetLinhas[i].str);

        free (arqCSV->vetLinhas);
    } 

    free (arqCSV);
    return NULL;
}

// fim funcoes de abertura e 
// fechamento/auxiliares de csvFile

// inicio funcoes de impressao/auxiliares

void tamMaxStrCols (csvFile *arqCSV)
{
    int i, numLins, j = 0;
    char buffer[TAM_MAX + 1], sep = ',';
    char *ptr;
    long len, tam;

    if (!arqCSV) return;

    for (i = 0; i < arqCSV->cols; i++) arqCSV->vetF[i] = TAM_PONTOS;
    numLins = (arqCSV->lins - 1) > LINS_FIX ? LINS_FIX : (arqCSV->lins - 1); 

    for (i = 0; i < numLins; i++, j = 0)
    {
        strcpy (buffer, arqCSV->vetLinhas[arqCSV->linsF[i]].str);
        tam = strlen (buffer);
        ptr = iniciaSeparadorStr (buffer, sep, tam);

        while (ptr)
        {
            len = strlen(ptr);
            if (len > arqCSV->vetF[j]) arqCSV->vetF[j] = len;
            ptr = separaStr (ptr, sep, &tam);
            j++;
        }
    }

    strcpy (buffer, arqCSV->vetLinhas[0].str);
    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);
    while (ptr)
    {
        len = strlen (ptr);
        if (len > arqCSV->vetF[j]) arqCSV->vetF[j] = len;
        ptr = separaStr (ptr, sep, &tam);
        j++;
    }
}

char *formataL (csvFile *arqCSV, char *str, char *linha, long lin)
{
    char *ptr, sep = ',', **cols;
    int col, pos = 0;
    long i, len, tam, tamNro, j = 0;

    cols = (char**)malloc (arqCSV->cols * sizeof (char*));
    if (!cols) return NULL;

    tam = strlen(str);
    ptr = iniciaSeparadorStr (str, sep, tam);
    while (ptr)
    {
        cols[j] = strdup(ptr);
        ptr = separaStr (ptr, sep, &tam);
        j++;
    }

    // posiciona o numero correspondente a linha na string formatada
    // coloca apenas espacos na string ao formatar o cabecalho
    if (lin >= 0)
    {
        sprintf (linha + pos, "%ld", lin);
        tamNro = tamanhoNro(lin);
        pos = tamanhoNro(lin);
    }
    else 
        tamNro = 0;   
    for (j = 0; j <= arqCSV->maxTamNro - tamNro; j++, pos++) 
        linha[pos] = ' ';

    // formata na string as demais colunas
    for (i = 0; i < arqCSV->tamSelecionaCols; i++)
    {
        col = arqCSV->selecionaCols[i];
        len = strlen(cols[col]);
        if (!len)
        {
            for (j = 0; j < arqCSV->vetF[col] - 3; j++, pos++) linha[pos] = ' ';
            strcpy (linha + pos, "Nan");
            pos = pos + 3;
        }
        else 
        {
            for (j = 0; j < arqCSV->vetF[col] - len; j++, pos++) linha[pos] = ' ';
            strcpy (linha + pos, cols[col]);
            pos = pos + len;
        }
        linha[pos] = ' ';
        pos++;
    }
    linha[pos - 1] = '\0';

    for (j = 0; j < arqCSV->cols; j++) free (cols[j]);
    free (cols);

    return linha;
}

void previaCSV (csvFile *arqCSV)
{
    char *ptr, *linha, buffer[TAM_MAX + 1], bufferAux[TAM_MAX + 1];
    long i, j, k, temp, tamNro;

    if (!arqCSV) return;

    tamMaxStrCols (arqCSV);
    if (arqCSV->lins <= LINS_FIX)
    {
        arqCSV->maxTamNro = TAM_LINS_FIX;
        strcpy (buffer, arqCSV->vetLinhas[0].str);
        formataL (arqCSV, buffer, bufferAux, -1);
        printf ("%s\n", bufferAux);

        for (i = 0; i < arqCSV->lins; i++)
        {
            strcpy (buffer, arqCSV->vetLinhas[arqCSV->linsF[i]].str);
            formataL (arqCSV, buffer, bufferAux, i);
            printf ("%s\n", bufferAux);
        }
    }
    else 
    {
        arqCSV->maxTamNro = tamanhoNro (arqCSV->lins - 2);
        if (arqCSV->maxTamNro < TAM_PONTOS) arqCSV->maxTamNro = TAM_PONTOS;
        strcpy (buffer, arqCSV->vetLinhas[0].str);
        formataL (arqCSV, buffer, bufferAux, -1);
        printf ("%s\n", bufferAux);

        for (i = 0; i < LINS_FIX - 5; i++)
        {
            strcpy (buffer, arqCSV->vetLinhas[arqCSV->linsF[i]].str);
            formataL (arqCSV, buffer, bufferAux, i);
            printf ("%s\n", bufferAux);
        }
        printf ("...");
        for (j = 0; j <= arqCSV->maxTamNro - TAM_PONTOS; j++) printf (" ");
        for (k = 0; k < arqCSV->tamSelecionaCols; k++)
        {
            temp = arqCSV->vetF[arqCSV->selecionaCols[k]];
            for (j = 0; j < temp - 3; j++) printf (" ");
            printf ("... ");    
        }
        printf ("\n");
        for (; i < LINS_FIX; i++) 
        {
            strcpy (buffer, arqCSV->vetLinhas[arqCSV->linsF[i]].str);
            formataL (arqCSV, buffer, bufferAux, arqCSV->lins - LINS_FIX + i - 1);
            printf ("%s\n", bufferAux);
        }
    }

    printf ("\n[%ld rows x %d columns]\n", arqCSV->lins - 1, arqCSV->cols);   
}

void sumarioCSV (csvFile *arqCSV)
{
    char *ptr, buffer[TAM_MAX + 1];
    char sep = ',';
    long tam;
    int i = 0;

    if (!arqCSV) return;

    strcpy (buffer, arqCSV->vetLinhas[0].str);
    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);

    while (ptr)
    {
        printf ("%s [%c]\n", ptr, arqCSV->tipos[i]);
        ptr = separaStr (ptr, sep, &tam); 
        i++;
    }
    printf ("\n%d variaveis encontradas\n", arqCSV->cols);
}

void imprimeDescricao (descricao d, char tipo)
{
    long i;

    if (tipo == 'N')
    {
        printf ("Contador: %ld\n", d.cont);
        printf ("Media: %.1f\n", d.mediana);
        printf ("Desvio: %.1f\n", d.desvioP);
        printf ("Mediana: %.1f\n", d.mediana);
    }
    printf ("Moda: %s ", d.moda);
    printf ("%ld vez(es)\n", d.modaQtd);
    if (tipo == 'N')
    {
        printf ("Min: %.1f\n", d.min);
        printf ("Max: %.1f\n", d.max); 
        printf ("Valores unicos: [");
        for (i = 0; i < (d.qtdUnicos - 1); i++) 
            printf ("%s,", d.col[d.unicos[i]]);
        if (i)
            printf ("%s", d.col[d.unicos[i]]); 
        printf ("]\n");      
    }
    else 
    {
        printf ("Valores unicos: [");
        for (i = 0; i < d.qtdUnicos; i++) 
            printf ("'%s',", d.col[d.unicos[i]]);
        if (i)
            printf ("'%s'", d.col[d.unicos[i]]); 
        printf ("]\n");  
    }
}

void selecionaLinsImpressao (csvFile *arqCSV)
{
    long i;

    if ((arqCSV->lins - 1) <= LINS_FIX) 
        for (i = 0; i < (arqCSV->lins - 1); i++) 
            arqCSV->linsF[i] = arqCSV->ordemLinhas[i + 1];
    else 
    {
        for (i = 0; i < LINS_FIX - 5; i++) 
            arqCSV->linsF[i] = arqCSV->ordemLinhas[i + 1];
        for (; i < LINS_FIX; i++) 
            arqCSV->linsF[i] = arqCSV->ordemLinhas[arqCSV->lins - LINS_FIX + i];
    }
}

// fim funcoes de impressao/auxiliares

long vetColunaS (csvFile *arqCSV, char **vet, int col)
{
    char buffer[TAM_MAX + 1];
    char *ptr;
    long i, j;

    for (i = 1, j = 0; i < arqCSV->lins; i++)
    {
        strcpy (buffer, arqCSV->vetLinhas[arqCSV->ordemLinhas[i]].str);
        ptr = obtemColuna (arqCSV, buffer, col);
        if (*ptr != '\0') 
        {
            vet[j] = strdup(ptr);
            j++;
        }
    }

    return j;
}

void calculoDescricao (csvFile *arqCSV, descricao *d, int col)
{
    char *ant, tipo = arqCSV->tipos[col];
    double aux;
    long double quadrados = 0;
    long *unicos, antQtd, i, j = 0;

    mergeSort (d->col, 0, d->cont - 1, arqCSV->ordemLinhas);
    
    // calcula media, variancia, moda e valores unicos
    antQtd = d->modaQtd = d->media = 0;
    d->moda = ant = d->col[0];
    d->min = d->max = atof(d->col[0]);
    for (i = 0; i < d->cont; i++) 
    {
        if (tipo == 'N')
        {
            aux = atof(d->col[arqCSV->ordemLinhas[i]]);
            if (aux < d->min) d->min = aux;
            else if (aux > d->max) d->max = aux;

            d->media += aux;
            quadrados += aux * aux;
        }

        if (strcmp(d->col[arqCSV->ordemLinhas[i]], ant))
        {
            if (antQtd > d->modaQtd)
            {
                d->modaQtd = antQtd;
                d->moda = ant;
            }
            if (antQtd == 1)
            {
                d->unicos[j] = arqCSV->ordemLinhas[i - 1];
                j++;
            }

            ant = d->col[arqCSV->ordemLinhas[i]];
            antQtd = 1;
        }
        else ++antQtd;
    }
    if (antQtd > d->modaQtd)
    {
        d->moda = ant;
        d->modaQtd = antQtd;
    }
    if (antQtd == 1) 
    {
        d->unicos[j] = arqCSV->ordemLinhas[i - 1];
        j++;
    }
    d->qtdUnicos = j;

    if (tipo == 'N')
    {
        d->media = d->media / d->cont;
        d->desvioP = sqrt ((quadrados - d->media * d->media * d->cont) / d->cont);
        if (d->cont % 2) 
            d->mediana = atof(d->col[d->cont / 2]);
        else 
            d->mediana = (atof(d->col[(d->cont - 1) / 2]) + atof(d->col[d->cont / 2])) / 2;
    }

    return;
}

void descricaoCSV (csvFile *arqCSV)
{
    descricao d;
    long i, j;
    char *ptr, **vetS, campo[TAM_MAX + 1];
    int col;

    if (!arqCSV) return;

    printf ("Entre com a variavel: ");
    obtemLinhaUser (campo);
    col = indiceColuna (arqCSV, campo);

    if (col < 0 || col >= arqCSV->cols) return;

    d.col = (char**)malloc (arqCSV->lins * sizeof(char*));
    if (!d.col)
    {
        msgFalhaAlocacao ();
        return;
    }

    for (j = 0; j < arqCSV->lins; j++) 
        arqCSV->ordemLinhas[j] = j;
    d.cont = vetColunaS (arqCSV, d.col, col);
    if (!d.cont) return;

    d.unicos = (long*)malloc (d.cont * sizeof (long));
    if (!d.unicos)
    {
        msgFalhaAlocacao ();
        free (d.col);
        return;
    }

    calculoDescricao (arqCSV, &d, col);
    imprimeDescricao (d, arqCSV->tipos[col]);

    for (i = 0; i < d.cont; i++) free (d.col[i]);
    free (d.unicos);
    free (d.col);
}

void selecionaCSV (csvFile *arqCSV)
{
    char sep = ' ', *ptr;
    char buffer[TAM_MAX + 1];
    long len, tam, i;
    int col, j = 0;
 
    printf ("Entre com a variaveis que deseja ");
    printf ("selecionar (separadas por espaço): ");
    obtemLinhaUser (buffer);
    
    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);
    while (ptr)
    {
        col = indiceColuna(arqCSV, ptr);
        if (col >= 0 && col < arqCSV->cols)
        {
            arqCSV->selecionaCols[j] = col;
            ++j;
        }

        ptr = separaStr (ptr, sep, &tam);
    }
    
    if (j)
    {
        for (i = 0; i < arqCSV->lins; i++) 
            arqCSV->ordemLinhas[i] = i;
        arqCSV->tamSelecionaCols = j;
        selecionaLinsImpressao (arqCSV);
        previaCSV (arqCSV);
        salvaEmArquivo (arqCSV);
    }
}

void mostraCSV (csvFile *arqCSV)
{
    long i;

    for (i = 0; i < arqCSV->lins; i++) 
        arqCSV->ordemLinhas[i] = i;
    selecionaLinsImpressao (arqCSV);

    for (i = 0; i < arqCSV->cols; i++) 
        arqCSV->selecionaCols[i] = i;
    arqCSV->tamSelecionaCols = arqCSV->cols;

    previaCSV (arqCSV);
}

char igual (char *str1, char *str2)
{

}

char diferente (char *str1, char *str2)
{
    
}

// char maiorIgual (char *str1, char *str2)
// {
    
// }
// estritamente maior
char maior (char *str1, char *str2)
{
    
}

char menorIgual (char *str1, char *str2)
{
    
}
// estritamente menor
char menor (char *str1, char *str2)
{
    
}

operacao selecionaFiltro (char *op)
{
    switch (op[0])
    {
        case '=':
            switch (op[1])
            {
                case '=':
                    return igual;
                    break;
                case '!': 
                    return diferente;
                    break;
                default:
                    printf ("Filtro invalido\n");
                    break;
            }
            break;
        case '>':
            switch (op[1])
            {
                case '=':
                    // return maiorIgual;   
                    break;
                case '\0': 
                    return maior;
                    break;
                default:
                    printf ("Filtro invalido\n");
                    break;
            }
            break;
        case '<':
            switch (op[1])
            {
                case '=':
                    return menorIgual;
                    break;
                case '\0': 
                    return menor;
                    break;
                default:
                    printf ("Filtro invalido\n");
                    break;
            }
            break;
        default:
            printf ("Filtro invalido\n");
            break;
    }
}

void filtraCSV (csvFile *arqCSV, char (*filtro)(char *, char *))
{
    
}

void ordenaCSV (csvFile *arqCSV)
{

}

void zerosCSV (csvFile *arqCSV)
{

}

int main ()
{
    csvFile* csv;
    char buffer[TAM_MAX + 1];
    char str[] = ",TESTE,TESTE,,TESTE,";
    char *ptr;
    long tam;

    // tam = strlen (str);
    // ptr = iniciaSeparadorStr (str, ',', tam);
    // while (ptr)
    // {
    //     printf ("%s\n", ptr);
    //     ptr = separaStr (ptr, ',', &tam);
    // }
    csv = abreCSV ("Teste2.csv");
    // for (int i = 0; i < LINS_FIX; i++) csv->linsF[i] = i;
    
    descricaoCSV (csv);
    // printf ("%s\n", csv->vetLinhas[1].str);
    fechaCSV (csv);
}

// FAZER DESTROI LISTA PARA ADICIONA EM ABRE