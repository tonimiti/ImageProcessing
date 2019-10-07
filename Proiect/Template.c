#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "Functions.h"

void mediePixeli(CULOAREINDEX **M, double *medie, int row, int col)
{
	*medie = 0;
	int i, j;
	for(i=0; i<row; i++)
		for(j=0; j<col; j++)
			*medie += M[i][j].R;
	*medie = *medie / (row*col);
}

void deviatieStandard(CULOAREINDEX **M, double medie, double *deviatie, int row, int col)
{
	*deviatie = 0;
	int i, j;
	for(i=0; i<row; i++)
		for(j=0; j<col; j++)
			*deviatie = *deviatie + ((M[i][j].R-medie)*(M[i][j].R-medie));
	*deviatie = *deviatie / (row*col-1);
	*deviatie = sqrt(*deviatie);
}

void getColor(char *cale_sablon, CULOAREINDEX *c)
{
	if(!strcmp(cale_sablon, "cifra0.bmp")) { (*c).R = 255; (*c).G = 0;   (*c).B = 0;   }
	if(!strcmp(cale_sablon, "cifra1.bmp")) { (*c).R = 255; (*c).G = 255; (*c).B = 0;   }
	if(!strcmp(cale_sablon, "cifra2.bmp")) { (*c).R = 0;   (*c).G = 255; (*c).B = 0;   }
	if(!strcmp(cale_sablon, "cifra3.bmp")) { (*c).R = 0;   (*c).G = 255; (*c).B = 255; }
	if(!strcmp(cale_sablon, "cifra4.bmp")) { (*c).R = 255; (*c).G = 0;   (*c).B = 255; }
	if(!strcmp(cale_sablon, "cifra5.bmp")) { (*c).R = 0;   (*c).G = 0;   (*c).B = 255; }
	if(!strcmp(cale_sablon, "cifra6.bmp")) { (*c).R = 192; (*c).G = 192; (*c).B = 192; }
	if(!strcmp(cale_sablon, "cifra7.bmp")) { (*c).R = 255; (*c).G = 140; (*c).B = 0;   }
	if(!strcmp(cale_sablon, "cifra8.bmp")) { (*c).R = 128; (*c).G = 0;   (*c).B = 128; }
	if(!strcmp(cale_sablon, "cifra9.bmp")) { (*c).R = 128; (*c).G = 0;   (*c).B = 0;   }
}

void templateMatching(char *cale_imagine, char *cale_sablon, double prag, int *nrDetectii, infoFereastra **detectii)
{
    grayscale_image(cale_sablon, "cifraGrey.bmp");

	unsigned char *header_imagine, *header_sablon;
    header_imagine = (unsigned char*)malloc(54*sizeof(unsigned char));
    if(header_imagine == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\templateMatching - header_imagine)\n");
        return;
    }
    header_sablon = (unsigned char*)malloc(54*sizeof(unsigned char));
    if(header_sablon == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\templateMatching - header_sablon)\n");
        return;
    }

    CULOAREINDEX *IMG, *SBL;
    int i, j;

    readBMP(cale_imagine, header_imagine, &IMG);
    readBMP("cifraGrey.bmp", header_sablon, &SBL);

	unsigned int latime_img, inaltime_img;
    latime_img = *(unsigned int*)&header_imagine[18];
    inaltime_img = *(unsigned int*)&header_imagine[22];
    latime_img = latime_img+(4 - (latime_img%4))%4;

    unsigned int latime_sbl, inaltime_sbl;
    latime_sbl = *(unsigned int*)&header_sablon[18];
    inaltime_sbl = *(unsigned int*)&header_sablon[22];
    latime_sbl = latime_sbl+(4 - (latime_sbl%4))%4;

    CULOAREINDEX **M_IMG, **M_SBL, **M_FRST;

    M_IMG = (CULOAREINDEX**)malloc(inaltime_img*sizeof(CULOAREINDEX*));
    for(i=0; i<inaltime_img; i++)
    	M_IMG[i] = (CULOAREINDEX*)malloc(latime_img*sizeof(CULOAREINDEX));
    for(i=0; i<inaltime_img; i++)
    	for(j=0; j<latime_img; j++)
    		M_IMG[i][j] = IMG[i*latime_img+j];
    if(M_IMG == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\templateMatching - M_IMG)\n");
        return;
    }

    M_SBL = (CULOAREINDEX**)malloc(inaltime_sbl*sizeof(CULOAREINDEX*));
    for(i=0; i<inaltime_sbl; i++)
    	M_SBL[i] = (CULOAREINDEX*)malloc(latime_sbl*sizeof(CULOAREINDEX));
    for(i=0; i<inaltime_sbl; i++)
    	for(j=0; j<latime_sbl; j++)
    		M_SBL[i][j] = SBL[i*latime_sbl+j];
    if(M_SBL == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\templateMatching - M_SBL)\n");
        return;
    }

    M_FRST = (CULOAREINDEX**)malloc(inaltime_sbl*sizeof(CULOAREINDEX*));
    	for(i=0; i<inaltime_sbl; i++)
    		M_FRST[i] = (CULOAREINDEX*)malloc(latime_sbl*sizeof(CULOAREINDEX));
    if(M_FRST == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\templateMatching - M_FRST)\n");
        return;
    }

    free(IMG); free(SBL);

    double medie_sablon, medie_fereastra, deviatie_sablon, deviatie_fereastra, corr=0;
    mediePixeli(M_SBL, &medie_sablon, inaltime_sbl, latime_sbl);
    deviatieStandard(M_SBL, medie_sablon, &deviatie_sablon, inaltime_sbl, latime_sbl);
    int x, y;
    int lastDetectii = *nrDetectii;

    for(x=0; x<inaltime_img-inaltime_sbl; x++)
    	for(y=0; y<latime_img-latime_sbl; y++)
    	{
    		for(i=0; i<inaltime_sbl; i++)
    			for(j=0; j<latime_sbl; j++)
    				M_FRST[i][j] = M_IMG[x+i][y+j];
    		mediePixeli(M_FRST, &medie_fereastra, inaltime_sbl, latime_sbl);
    		deviatieStandard(M_FRST, medie_fereastra, &deviatie_fereastra, inaltime_sbl, latime_sbl);
    		for(i=0; i<inaltime_sbl; i++)
    			for(j=0; j<latime_sbl; j++)
    				corr = corr + ((M_FRST[i][j].R - medie_fereastra)*(M_SBL[i][j].R - medie_sablon)/(deviatie_sablon*deviatie_fereastra));
    		corr = corr/(inaltime_sbl*latime_sbl);
    		if(corr > prag)
    		{
    			*detectii = (infoFereastra*)realloc(*detectii, (lastDetectii+1)*sizeof(infoFereastra));
    			(*detectii)[lastDetectii].xpos_inc = x;
    			(*detectii)[lastDetectii].xpos_final = x+inaltime_sbl;
    			(*detectii)[lastDetectii].ypos_inc = y;
    			(*detectii)[lastDetectii].ypos_final = y+latime_sbl;
    			(*detectii)[lastDetectii].corr = corr;
    			getColor(cale_sablon, &(*detectii)[lastDetectii].color);
    			lastDetectii++;
    		}
    		corr=0;
    	}
    *nrDetectii = lastDetectii;

    for(i=0; i<inaltime_img; i++)
        free(M_IMG[i]);
    free(M_IMG);

    for(i=0; i<inaltime_sbl; i++)
        free(M_SBL[i]);
    free(M_SBL);

    for(i=0; i<inaltime_sbl; i++)
        free(M_FRST[i]);
    free(M_FRST);

    free(header_imagine); free(header_sablon);
}

int overlap(infoFereastra a, infoFereastra b)
{
	if(a.xpos_inc > b.xpos_final || b.xpos_inc > a.xpos_final)
		return 0;
	if(a.ypos_inc > b.ypos_final || b.ypos_inc > a.ypos_final)
		return 0;
	return 1;
}

int minim(int a, int b)
{
	if(a<b)
		return a;
	return b;
}

int maxim(int a, int b)
{
	if(a<b)
		return b;
	return a;
}

double overlapArea(infoFereastra a, infoFereastra b)
{
	double area_a, area_b, area_intersection;
	area_a = abs(a.xpos_inc - a.xpos_final) * abs(a.ypos_inc - a.ypos_final);
	area_a = abs(b.xpos_inc - b.xpos_final) * abs(b.ypos_inc - b.ypos_final);
	area_intersection = (minim(a.xpos_final, b.xpos_final) - maxim(a.xpos_inc, b.xpos_inc))	* (minim(a.ypos_final, b.ypos_final) - maxim(a.ypos_inc, b.ypos_inc));
	return (area_intersection/(area_a + area_b - area_intersection));
}

void delete_nonmaxim(infoFereastra **detectii, int *nrDetectii)
{
	double suprapunere;
	unsigned int i, j, k;

	for(i = 0; i < (*nrDetectii)-1; i++)
		for(j = i+1; j < (*nrDetectii); j++)
		{
			if(overlap((*detectii)[i], (*detectii)[j]))
			{
				suprapunere = overlapArea((*detectii)[i], (*detectii)[j]);
				if(suprapunere > 0.20)
				{
					for(k = j; k < (*nrDetectii)-1; k++)
						(*detectii)[k] = (*detectii)[k+1];
					(*nrDetectii)--;
					j--;
				}
			}
		}
}

void contur(CULOAREINDEX ***M, CULOAREINDEX culoare, infoFereastra f)
{
	int i;
	for(i=f.ypos_inc; i<f.ypos_final; i++)
	{
		(*M)[f.xpos_inc][i] = culoare;
		(*M)[f.xpos_final-1][i] = culoare;
	}
	for(i=f.xpos_inc; i<f.xpos_final; i++)
	{
		(*M)[i][f.ypos_inc] = culoare;
		(*M)[i][f.ypos_final-1] = culoare;
	}
}

int cmpCorelatii(const void *a, const void *b)
{
	if(((infoFereastra*)a)->corr > ((infoFereastra*)b)->corr)
		return -1;
	return +1;
}

void colorareImg(char *cale_imagine)
{
	unsigned int i, j;
	int nrDetectii = 0;
	infoFereastra *detectii;
	detectii = (infoFereastra*)malloc(sizeof(infoFereastra));
	if(detectii == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\colorareImg - detectii)\n");
        return;
    }

	grayscale_image(cale_imagine, "grey.bmp");

	int nrSabloane;
    char *sablon;
    sablon = (char*)calloc(20, sizeof(char));
    CULOAREINDEX c;
    FILE *fin = fopen("Sabloane.txt", "rt");
    if(fin == NULL)
    {
        printf("FILE ERROR (Template.c\\colorareIMG - Sabloane.txt)\n");
        return;
    }

    fscanf(fin, "%d", &nrSabloane);
    for(i = 0; i < nrSabloane; i++)
    {
        fscanf(fin, "%s", sablon);
        templateMatching("grey.bmp", sablon, 0.50, &nrDetectii, &detectii);
    }
    fclose(fin);

	qsort(detectii, nrDetectii, sizeof(infoFereastra), cmpCorelatii);
	delete_nonmaxim(&detectii, &nrDetectii);
	detectii = (infoFereastra*)realloc(detectii, (nrDetectii+1)*sizeof(infoFereastra));

	unsigned char *header_nou;
    header_nou = (unsigned char*)malloc(54*sizeof(unsigned char));
    if(header_nou == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\colorareImg - header_nou)\n");
        return;
    }

    CULOAREINDEX *IMGN;
    readBMP(cale_imagine, header_nou, &IMGN);

    unsigned int latime_img, inaltime_img;
    latime_img = *(unsigned int*)&header_nou[18];
    inaltime_img = *(unsigned int*)&header_nou[22];
    latime_img = latime_img+(4 - (latime_img%4))%4;

    CULOAREINDEX **M_IMGN;
    M_IMGN = (CULOAREINDEX**)malloc(inaltime_img*sizeof(CULOAREINDEX*));
    for(i=0; i<inaltime_img; i++)
    	M_IMGN[i] = (CULOAREINDEX*)malloc(latime_img*sizeof(CULOAREINDEX));
    for(i=0; i<inaltime_img; i++)
    	for(j=0; j<latime_img; j++)
    		M_IMGN[i][j] = IMGN[i*latime_img+j];
    if(M_IMGN == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (Template.c\\colorareImg - M_IMGN)\n");
        return;
    }

    for(i=0; i<nrDetectii; i++)
    {
    	c = detectii[i].color;
    	contur(&M_IMGN, c, detectii[i]);
	}
    for(i=0; i<inaltime_img; i++)
     	for(j=0; j<latime_img; j++)
    		IMGN[i*latime_img+j] = M_IMGN[i][j];

    storeBMP("ColoredImg.bmp", header_nou, IMGN);

    free(detectii); free(IMGN); free(header_nou);
    for(i=0; i<inaltime_img; i++)
        free(M_IMGN[i]);
    free(M_IMGN);
}
