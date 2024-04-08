#include "io.h"
#include "linkedList.h"
#include "ordenacao.h"

// inicio funcoes auxiliares globais

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
    long tam, len;
    char *ptr, sep = ',';

    if (!arqCSV || col < 0 || col > arqCSV->cols) 
        return NULL;

    tam = strlen (buffer);
    ptr = separaStr (buffer, sep, tam);
    for (i = 0; i < col; i++)
    {
        len = strlen (ptr);
        tam = tam - len - 1;
        ptr = separaStr (ptr + len + 1, sep, tam);
    }

    return ptr;
}

// fim funcoes auxiliares globais

// inicio funcoes de abertura e 
// fechamento/auxiliares de csvFile

char verificaCabecalho (csvFile* arqCSV, char* buffer)
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
    arqCSV->tipos = NULL;
    arqCSV->vetF = NULL;
    arqCSV->ordemLinhas = NULL;
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
    int tam, i, j;
    size_t len;
    
    arqCSV = alocaCSV ();
    arqCSV->arq = fopen (pathArquivoCSV, "r");
    lTemp = criaLista ();
    if (!arqCSV->arq || !lTemp) 
        return fechaCSV (arqCSV);

    if (!obtemLinhaArq (arqCSV->arq, buffer)) 
        return fechaCSV (arqCSV);
    if (!verificaCabecalho (arqCSV, buffer)) 
        return fechaCSV (arqCSV);
    insereLista (criaNodoStr (buffer, 1), lTemp);
    ++arqCSV->lins;

    arqCSV->tipos = (char*)malloc (arqCSV->cols * sizeof (char));   
    arqCSV->vetF = (short int*)malloc (arqCSV->cols * sizeof (short int));
    arqCSV->selecionaCols = (char*)malloc (arqCSV->cols);
    if (!arqCSV->tipos || !arqCSV->vetF || !arqCSV->selecionaCols) 
        return fechaCSV (arqCSV);

    for (j = 0; j < arqCSV->cols; j++) 
        arqCSV->tipos[j] = 'N';

    erro = 0;
    while (obtemLinha (arqCSV->arq, buffer) && !erro)
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
        return fechaCSV (arqCSV);

    arqCSV->vetLinhas = (linha*)malloc (arqCSV->lins * sizeof (linha));
    arqCSV->ordemLinhas = (long*)malloc ((arqCSV->lins - 1) * sizeof (long));
    if (!arqCSV->vetLinhas || !arqCSV->ordemLinhas) 
        return fechaCSV (arqCSV);

    for (j = 0; j < arqCSV->lins; j++) 
        arqCSV->ordemLinhas[j] = j;
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
    int i, tam, numLins, j = 0;
    char buffer[TAM_MAX + 1], sep = ',';
    char *ptr;
    long len;

    if (!arqCSV) return;

    for (i = 0; i < arqCSV->cols; i++) arqCSV->vetF[i] = TAM_PONTOS;
    numLins = arqCSV->lins > LINS_FIX ? LINS_FIX : arqCSV->lins; 

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
    char *ptr, sep = ',';
    int pos = 0;
    long i, j, len, tam, tamNro;

    tam = strlen(str);
    ptr = iniciaSeparadorStr (str, sep, tam);

    sprintf (linha + pos, "%ld", lin);
    pos = tamanhoNro(lin);
    for (j = 0; j <= arqCSV->maxTamNro - tamanhoNro(lin); j++, pos++) 
        linha[pos] = ' ';

    for (i = 0; i < arqCSV->cols; i++)
    {
        if (arqCSV->selecionaCols[i])
        {
            len = strlen(ptr);
            if (!len)
            {
                for (j = 0; j < arqCSV->vetF[i] - 3; j++, pos++) linha[pos] = ' ';
                strcpy (linha + pos, "Nan");
                pos = pos + 3;
            }
            else 
            {
                for (j = 0; j < arqCSV->vetF[i] - len; j++, pos++) linha[pos] = ' ';
                strcpy (linha + pos, ptr);
                pos = pos + len;
            }
            linha[pos] = ' ';
            pos++;
        }

        ptr = separaStr (ptr, sep, &tam);
    }
    linha[pos - 1] = '\0';

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
        arqCSV->maxTamNro = tamanhoNro (LINS_FIX - 1);
        formataL (arqCSV, buffer, bufferAux, -1);
        for (i = 0; i < arqCSV->lins; i++)
        {
            strcpy (buffer, arqCSV->vetLinhas[arqCSV->linsF[i]].str);
            formataL (arqCSV, buffer, bufferAux, i);
            printf ("%s\n", bufferAux);
        }
    }
    else 
    {
        arqCSV->maxTamNro = tamanhoNro (arqCSV->lins);
        if (arqCSV->maxTamNro < TAM_PONTOS) arqCSV->maxTamNro = TAM_PONTOS;
        formataL (arqCSV, buffer, bufferAux, -1);
        for (i = 0; i < LINS_FIX - 5; i++)
        {
            strcpy (buffer, arqCSV->vetLinhas[arqCSV->linsF[i]].str);
            formataL (arqCSV, buffer, bufferAux, i);
            printf ("%s\n", bufferAux);
        }
        printf ("...");
        for (j = 0; j <= arqCSV->maxTamNro - TAM_PONTOS; j++) printf (" ");
        for (k = 0; k < arqCSV->cols; k++)
        {
            if (arqCSV->selecionaCols[k])
            {
                temp = arqCSV->vetF[k];
                for (j = 0; j < temp - 3; j++) printf (" ");
                printf ("... ");
            }    
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

// fim funcoes de impressao/auxiliares

void copiaListaLemVet (struct listaLigada *l, linha *vet)
{
    void *ptr;
    char s;
    long i = 0;

    while (removeListaL (l, &ptr, &s))
    {
        vet[i].str = (char*)ptr;
        vet[i].completa = s;
        i++;
    }     
}

long vetColunaS (csvFile *arqCSV, char ***vetS, int col)
{
    char buffer[TAM_MAX + 1];
    char **vet, *ptr;
    long i, j;

    vet = (char**)malloc (arqCSV->lins * sizeof (char*));
    if (!vet) return 0;

    for (i = 1, j = 0; i < arqCSV->lins; i++)
    {
        strcpy (buffer, arqCSV->vetLinhas[i].str);
        ptr = coluna (arqCSV, buffer, col);
        if (*ptr != '\0') 
        {
            vet[j] = strdup(ptr);
            j++;
        }
    }

    *vetS = vet;

    return j;
}

void descricaoNumeric (csvFile *arqCSV, int col)
{
    long double quadrados = 0;
    double *vetN, *unicosN; 
    double mediana, moda, dp, min, max, atual, media = 0;
    long tam, qtdAtual, qtdModa, i, j;

    tam = vetColunaN (arqCSV, &vetN, col);
    if (!vetN) return;
    unicosN = (double*)malloc (tam * sizeof (double));
    if (!unicosN) return;
    mergeSortN (vetN, tam);
    
    qtdAtual = qtdModa = 0;
    moda = atual = min = max = vetN[0];
    for (i = 0, j = 0; i < tam; i++) 
    {
        if (vetN[i] < min) min = vetN[i];
        else if (vetN[i] > max) max = vetN[i];

        media += vetN[i];
        quadrados += vetN[i] * vetN[i];

        if (vetN[i] != atual) 
        {
            if (qtdAtual > qtdModa)
            {
                qtdModa = qtdAtual;
                moda = atual;
            }
            if (qtdAtual == 1)
            {
                unicosN[j] = atual;
                j++;
            }

            atual = vetN[i];
            qtdAtual = 1;
        }
        else ++qtdAtual;
    }
    if (qtdAtual > qtdModa)
    {
        qtdModa = qtdAtual;
        moda = atual;
    }
    if (qtdAtual == 1)
    {
        unicosN[j] = atual;
        j++;
    }

    media = media / tam;
    dp = (quadrados - media * media * tam) / tam;

    if (tam % 2) mediana = vetN[tam / 2];
    else mediana = (vetN[(tam - 1) / 2] + vetN[tam / 2]) / 2;

    printf ("Contador: %ld\n", tam);
    printf ("Media: %.1f\n", media);
    printf ("Desvio: %.1f\n", dp);
    printf ("Mediana: %.1f\n", mediana);
    printf ("Moda: %.1f ", moda);
    printf ("%ld vez(es)\n", qtdModa);
    printf ("Min: %.1f\n", min);
    printf ("Max: %.1f\n", max);
    imprimeValoresUnicosN (unicosN, j);
    
    free (unicosN);
    free (vetN);
}

void descricaoString (csvFile *arqCSV, int col)
{
    char *moda, *atual, **unicosS;
    long tam, qtdAtual, qtdModa, i, j;
    char *ptr, **vetS, campo[TAM_MAX + 1];

    tam = vetColunaS (arqCSV, &vetS, col);
    mergeSortStr (vetS, tam);
    if (!vetS) return;
    unicosS = (char**)malloc (tam * sizeof (char*));
    if (!unicosS) return;

    qtdModa = qtdAtual = 0;
    atual = vetS[0];
    for (i = 1, j = 0; i < tam; i++)
    {
        if (strcmp (vetS[i], atual))
        {
            if (qtdAtual > qtdModa)
            {
                moda = atual;
                qtdModa = qtdAtual;
            }
            if (qtdAtual == 1) 
            {
                unicosS[j] = atual;
                j++;
            }

            qtdAtual = 1;
            atual = vetS[i];
        }
        else ++qtdAtual;
    }
    if (qtdAtual > qtdModa)
    {
        moda = atual;
        qtdModa = qtdAtual;
    }
    if (qtdAtual == 1) 
    {
        unicosS[j] = atual;
        j++;
    }

    printf ("Contador: %ld\n", tam);
    printf ("Moda: %s ", moda);
    printf ("%ld vez(es)\n", qtdModa);
    printf ("[");
    for (i = 0; i < j - 1; i++) printf ("'%s',", unicosS[i]);
    printf ("%s]", unicosS[i]);

    for (i = 0; i < tam; i++) free (vetS[i]);
    free (unicosS);
    free (vetS);
}

void descricaoCSV (csvFile *arqCSV)
{
    long tam, qtdAtual, qtdModa, i, j;
    char *ptr, **vetS, campo[TAM_MAX + 1];
    int col;

    if (!arqCSV) return;

    printf ("Entre com a variavel: ");
    obtemLinhaUser (campo);
    col = achaColuna (arqCSV, campo);

    if (col < 0 || col >= arqCSV->cols) return;

    if (arqCSV->tipos[col] == 'N')
        descricaoNumeric (arqCSV, col);
    else 
        descricaoString (arqCSV, col);
}

void salvaEmArquivo (csvFile *arqCSV)
{
    FILE *arq;
    char buffer[TAM_MAX + 1];
    char sep = ',', *ptr;
    long i, k, j;

    printf ("Deseja gravar um arquivo com ");
    printf ("as variáveis selecionadas? [S|N] ");
    obtemLinhaUser (buffer);

    if (strlen(buffer) == 1 && buffer[0] == 'S')
    {
        printf ("Entre com o nome do arquivo: ");
        obtemLinhaUser (buffer);

        arq = fopen (buffer, "w");
        if (!arq) return;

        for (i = 0; i < arqCSV->lins; i++)
        {
            for (j = 0; j < arqCSV->cols - 1; j++)
            {
                if (arqCSV->selecionaCols[j])
                {
                    strcpy (buffer, arqCSV->vetLinhas[i].str);
                    ptr = coluna (arqCSV, buffer, j);
                    fprintf (arq, "%s,", ptr);
                }
            }
            if (arqCSV->selecionaCols[j])
            {
                strcpy (buffer, arqCSV->vetLinhas[i].str);
                ptr = coluna (arqCSV, buffer, j);
                fprintf (arq, "%s\n", ptr);
            }
        }

        fclose (arq);
    }
}

void selecionaCSV (csvFile *arqCSV)
{
    char sep = ' ', *ptr;
    char campo[TAM_MAX + 1], buffer[TAM_MAX + 1];
    long len, tam;
    int col;
 
    printf ("Entre com a variaveis que deseja ");
    printf ("selecionar (separadas por espaço): ");
    obtemLinhaUser (campo);

    memset (arqCSV->selecionaCols, 0, arqCSV->cols);
    
    tam = strlen (buffer);
    ptr = iniciaSeparadorStr (buffer, sep, tam);
    while (ptr)
    {
        col = achaColuna(arqCSV, ptr);
        if (col >= 0 && col < arqCSV->cols)
            arqCSV->selecionaCols[col] = 1;

        ptr = separaStr (ptr, sep, &tam);
    }

    previaCSV (arqCSV);

    salvaEmArquivo (arqCSV);
}

void mostraCSV (csvFile *arqCSV)
{
    int i;

    if (arqCSV->lins <= LINS_FIX) 
        for (i = 0; i < arqCSV->lins; i++) 
            arqCSV->linsF[i] = i + 1;
    else 
    {
        for (i = 0; i < LINS_FIX - 5; i++) 
            arqCSV->linsF[i] = i + 1;
        for (i = LINS_FIX - 5; i > 0; i--) 
            arqCSV->linsF[i] = arqCSV->lins - i;
    }

    for (i = 0; i < arqCSV->cols; i++) arqCSV->selecionaCols[i] = 1;

    previaCSV (arqCSV);
}

void filtraCSV (csvFile *arqCSV, char (*filtro)(char *, char *));
void ordenaCSV (csvFile *arqCSV);
void zerosCSV (csvFile *arqCSV);

int main ()
{
    csvFile* csv;
    char buffer[TAM_MAX + 1];

    csv = abreCSV ("Teste2.csv");
    for (int i = 0; i < LINS_FIX; i++) csv->linsF[i] = i;
    
    mostraCSV (csv);
    fechaCSV (csv);
}

// TRATAR CASO NAO HAJA NENHUM VALOR VALIDO NA COLUNA (DESCRICAOCSV)