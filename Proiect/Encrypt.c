#include <stdlib.h>
#include <stdio.h>
#include "Functions.h"

unsigned int XORSHIFT32(unsigned int x)
{
    x = x ^ x << 13;
    x = x ^ x >> 17;
    x = x ^ x << 5;
    return x;
}

void generate(unsigned int **R, unsigned int **permutare, unsigned int nrElemR)
{
    unsigned int i;

    for(i=1; i<nrElemR; i++)
        (*R)[i] = XORSHIFT32((*R)[i-1]);

    *permutare = (unsigned int*)malloc((nrElemR/2)*sizeof(unsigned int));
    if((*permutare) == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\generate - permutare)\n");
        return;
    }

    for(i=0; i<nrElemR/2; i++)
        (*permutare)[i] = i;

    unsigned int aux, r, k;
    k=1;
    for(i=(nrElemR/2)-1; i>0; i--) //generez permutarea Durstenfeld
    {
        r = (*R)[k]%(i+1);
        k++;
        aux = (*permutare)[r];
        (*permutare)[r] = (*permutare)[i];
        (*permutare)[i] = aux;
    }
}

void encryptBMP(char *cale_imagine, char *cale_imagine_criptata, char *cale_cheie, unsigned char *header, CULOAREINDEX *L)
{
    unsigned int latime_img, inaltime_img;
    latime_img = *(unsigned int*)&header[18];
    inaltime_img = *(unsigned int*)&header[22];
    latime_img = latime_img+(4 - (latime_img%4))%4;
    unsigned int nrElemR = 2*inaltime_img*latime_img;
    unsigned int *R, *permutare;
    R = (unsigned int*)malloc(nrElemR*sizeof(unsigned int));
    if(R == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\encryptBMP - R)\n");
        return;
    }

    FILE * in = fopen(cale_cheie, "rt");
    if(in == NULL)
    {
        printf("FILE ERROR (Encrypt.c\\encryptBMP - %s)\n", cale_cheie);
        return;
    }
    unsigned int sv;
    fscanf(in, "%u%u", &R[0],&sv);
    fclose(in);

    generate(&R, &permutare, nrElemR);

    unsigned int i;

    CULOAREINDEX *LPrim;
    LPrim = (CULOAREINDEX*)malloc((latime_img*inaltime_img)*sizeof(CULOAREINDEX));
    if(LPrim == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\encryptBMP - LPrim)\n");
        return;
    }

    for(i=0; i<latime_img*inaltime_img; i++) //permut pixelii imaginii initiale
        LPrim[permutare[i]] = L[i];

    CULOAREINDEX *C;
    C = (CULOAREINDEX*)malloc((latime_img*inaltime_img)*sizeof(CULOAREINDEX));
    if(C == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\encryptBMP - C)\n");
        return;
    }

    unsigned char X0, X1, X2, auxiliar;
    X0 = sv; //salvez primul octet din sv
    X1 = (sv>>8); //al doielea
    X2 = (sv>>16); //al treielea
    auxiliar = R[latime_img*inaltime_img]; //acelasi lucru pentru elementele din R
    C[0].B = LPrim[0].B ^ X0 ^ auxiliar; //xorez dupa algoritmul prezentat in enunt
    auxiliar = (R[latime_img*inaltime_img]>>8);
    C[0].G = LPrim[0].G ^ X1 ^ auxiliar;
    auxiliar = (R[latime_img*inaltime_img]>>16);
    C[0].R = LPrim[0].R ^ X2 ^ auxiliar;
    for(i=1; i<latime_img*inaltime_img; i++)
    {
        auxiliar = R[latime_img*inaltime_img+i];
        C[i].B = LPrim[i].B ^ C[i-1].B ^ auxiliar;
        auxiliar = (R[latime_img*inaltime_img+i]>>8);
        C[i].G = LPrim[i].G ^ C[i-1].G ^ auxiliar;
        auxiliar = (R[latime_img*inaltime_img+i]>>16);
        C[i].R = LPrim[i].R ^ C[i-1].R ^ auxiliar;
    }

    storeBMP(cale_imagine_criptata, header, C);

    free(R); free(C); free(permutare); free(LPrim);
}

void decryptBMP(char *cale_imagine, char *cale_imagine_decriptata, char *cale_cheie, unsigned char *header, CULOAREINDEX *C)
{
    unsigned int latime_img, inaltime_img;
    latime_img = *(unsigned int*)&header[18];
    inaltime_img = *(unsigned int*)&header[22];
    latime_img = latime_img+(4 - (latime_img%4))%4;
    unsigned int nrElemR = 2*inaltime_img*latime_img;
    unsigned int *R, *permutare;
    R = (unsigned int*)malloc(nrElemR*sizeof(unsigned int));
    if(R == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\decryptBMP - R)\n");
        return;
    }

    FILE * in = fopen(cale_cheie, "rt");
    if(in == NULL)
    {
        printf("FILE ERROR (Encrypt.c\\decryptBMP - %s)\n", cale_cheie);
        return;
    }
    unsigned int sv;
    fscanf(in, "%u%u", &R[0],&sv);
    fclose(in);

    generate(&R, &permutare, nrElemR);

    unsigned int i;
    unsigned char X0, X1, X2, auxiliar;

    CULOAREINDEX *CPrim;
    CPrim = (CULOAREINDEX*)malloc((latime_img*inaltime_img)*sizeof(CULOAREINDEX));
    if(CPrim == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\decryptBMP - CPrim)\n");
        return;
    }

    X0 = sv;
    X1 = (sv>>8);
    X2 = (sv>>16);
    auxiliar = R[latime_img*inaltime_img];
    CPrim[0].B = C[0].B ^ X0 ^ auxiliar;
    auxiliar = (R[latime_img*inaltime_img]>>8);
    CPrim[0].G = C[0].G ^ X1 ^ auxiliar;
    auxiliar = (R[latime_img*inaltime_img]>>16);
    CPrim[0].R = C[0].R ^ X2 ^ auxiliar;
    for(i=1; i<latime_img*inaltime_img; i++)
    {
        auxiliar = R[latime_img*inaltime_img+i];
        CPrim[i].B = C[i].B ^ C[i-1].B ^ auxiliar;
        auxiliar = (R[latime_img*inaltime_img+i]>>8);
        CPrim[i].G = C[i].G ^ C[i-1].G ^ auxiliar;
        auxiliar = (R[latime_img*inaltime_img+i]>>16);
        CPrim[i].R = C[i].R ^ C[i-1].R ^ auxiliar;
    }

    CULOAREINDEX *D;
    D = (CULOAREINDEX*)malloc((latime_img*inaltime_img)*sizeof(CULOAREINDEX));
    if(D == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\decryptBMP - D)\n");
        return;
    }

    for(i=0; i<inaltime_img*latime_img; i++)
        D[i] = CPrim[permutare[i]];

    storeBMP(cale_imagine_decriptata, header, D);

    free(R); free(CPrim); free(permutare); free(D);
}

void test_chi2(char *cale_imagine)
{
    unsigned char *header;
    CULOAREINDEX *L;
    header = (unsigned char*)malloc(54*sizeof(unsigned char));
    if(header == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\test_chi2 - header)\n");
        return;
    }

    readBMP(cale_imagine, header, &L);

    printf("Testul chi pentru imaginea: %s \n", cale_imagine);

    double th_frequency;
    unsigned int latime_img, inaltime_img;
    latime_img = *(unsigned int*)&header[18];
    inaltime_img = *(unsigned int*)&header[22];
    latime_img = latime_img+(4 - (latime_img%4))%4;
    th_frequency = (latime_img*inaltime_img) / 256;
    double *chi2 = (double*)calloc(3, sizeof(double));
    unsigned int *red = (unsigned int*)calloc(256, sizeof(unsigned int));
    if(red == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\test_chi2 - red)\n");
        return;
    }

    unsigned int *green = (unsigned int*)calloc(256, sizeof(unsigned int));
    if(green == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\test_chi2 - green)\n");
        return;
    }

    unsigned int *blue = (unsigned int*)calloc(256, sizeof(unsigned int));
    if(blue == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Encrypt.c\\test_chi2 - blue)\n");
        return;
    }

    unsigned int i;
    for(i=0; i<latime_img*inaltime_img; i++)
    {
        red[L[i].R]++;
        green[L[i].G]++;
        blue[L[i].B]++;
    }
    for(i=0; i<256; i++)
    {
        chi2[0] = chi2[0] + ((red[i]-th_frequency)*(red[i]-th_frequency))/th_frequency;
        chi2[1] = chi2[1] + ((green[i]-th_frequency)*(green[i]-th_frequency))/th_frequency;
        chi2[2] = chi2[2] + ((blue[i]-th_frequency)*(blue[i]-th_frequency))/th_frequency;
    }
    printf("RED: %.2f\nGREEN: %.2f\nBLUE: %.2f\n\n", chi2[0], chi2[1], chi2[2]);

    free(chi2); free(red); free(blue); free(green);
}
