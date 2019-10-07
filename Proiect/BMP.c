#include <stdlib.h>
#include <stdio.h>
#include "Functions.h"

void readBMP(char *cale_imagine, unsigned char *header, CULOAREINDEX **L)
{
    FILE *fin = fopen(cale_imagine, "rb");
    if(fin == NULL)
    {
        printf("FILE ERROR (BMP.c\\readBMP - %s)\n", cale_imagine);
        return;
    }

    unsigned int latime_img, inaltime_img;

    fread(header, sizeof(unsigned char), 54, fin);
    latime_img = *(unsigned int*)&header[18];
    inaltime_img = *(unsigned int*)&header[22];
    latime_img = latime_img+(4 - (latime_img%4))%4; 

    *L = (CULOAREINDEX*)malloc((latime_img*inaltime_img)*sizeof(CULOAREINDEX));
    if(L == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (BMP.c\\readBMP - L)\n");
        return;
    }

    int i, j;
    for(i=inaltime_img-1; i>=0; i--)
        for(j=0; j<latime_img; j++)
        {
            fread(&(*L)[i*latime_img+j].B, sizeof(unsigned char), 1, fin);
            fread(&(*L)[i*latime_img+j].G, sizeof(unsigned char), 1, fin);
            fread(&(*L)[i*latime_img+j].R, sizeof(unsigned char), 1, fin);
        }

    fclose(fin);
}

void storeBMP(char *cale_imagine, unsigned char *header, CULOAREINDEX *L)
{
    FILE *fout = fopen(cale_imagine, "wb");
    if(fout == NULL)
    {
        printf("FILE ERROR (BMP.c\\storeBMP - %s)\n", cale_imagine);
        return;
    }

    fwrite(header, sizeof(unsigned char), 54, fout);

    unsigned int latime_img, inaltime_img;
    latime_img = *(unsigned int*)&header[18];
    inaltime_img = *(unsigned int*)&header[22];
    latime_img = latime_img+(4 - (latime_img%4))%4;

    int i, j;
    for(i=inaltime_img-1; i>=0; i--)
        for(j=0; j<latime_img; j++)
        {
            fwrite(&L[i*latime_img+j].B, sizeof(unsigned char), 1, fout);
            fwrite(&L[i*latime_img+j].G, sizeof(unsigned char), 1, fout);
            fwrite(&L[i*latime_img+j].R, sizeof(unsigned char), 1, fout);
        }

    fclose(fout);
}

void grayscale_image(char *nume_fisier_sursa, char *nume_fisier_destinatie)
{
   FILE *fin, *fout;
   unsigned int dim_img, latime_img, inaltime_img;
   unsigned char pRGB[3], header[54], aux;

   printf("nume_fisier_sursa = %s \n",nume_fisier_sursa);

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);

   fseek(fin, 18, SEEK_SET);
   fread(&latime_img, sizeof(unsigned int), 1, fin);
   fread(&inaltime_img, sizeof(unsigned int), 1, fin);

   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);

	//calculam padding-ul pentru o linie
	int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    //printf("padding = %d \n",padding);

	fseek(fout, 54, SEEK_SET);
	int i,j;
	for(i = 0; i < inaltime_img; i++)
	{
		for(j = 0; j < latime_img; j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
			aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
			pRGB[0] = pRGB[1] = pRGB[2] = aux;
        	fseek(fout, -3, SEEK_CUR);
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout,padding,SEEK_CUR);
	}
	fclose(fout);
}