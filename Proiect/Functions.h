#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

typedef struct
{
    unsigned char B, G, R;
}CULOAREINDEX;

typedef struct
{
	int xpos_inc, xpos_final, ypos_inc, ypos_final;
	CULOAREINDEX color;
	double corr;
}infoFereastra;

//Functii pentru liniarizare BMP, stocare in memoria externa si grayscale
void readBMP(char *cale_imagine, unsigned char *header, CULOAREINDEX **L); //File: BMP.c; Line: 5
void storeBMP(char *cale_imagine, unsigned char *header, CULOAREINDEX *L); //File: BMP.c; Line: 40
void grayscale_image(char *nume_fisier_sursa, char *nume_fisier_destinatie); //File: BMP.c; Line: 68

//Functii pentru criptarea/decriptarea unei imaginii BMP
unsigned int XORSHIFT32(unsigned int x); //File: Encrypt.c; Line: 5
void generate(unsigned int **R, unsigned int **permutare, unsigned int nrElemR); //File: Encrypt.c; Line: 13
void encryptBMP(char *cale_imagine, char *cale_imagine_criptata, char *cale_cheie, unsigned char *header, CULOAREINDEX *L); //File: Encrypt.c; Line: 42
void decryptBMP(char *cale_imagine, char *cale_imagine_decriptata, char *cale_cheie, unsigned char *header, CULOAREINDEX *C); //File: Encrypt.c; Line: 115
void test_chi2(char *cale_imagine); //File: Encrypt.c; Line: 188

//Functii pentru template matching intre o imagine si un sablon
void mediePixeli(CULOAREINDEX **M, double *medie, int row, int col); //File: Template.c; Line: 7
void deviatieStandard(CULOAREINDEX **M, double medie, double *deviatie, int row, int col); //File: Template.c; Line: 17
void getColor(char *cale_sablon, CULOAREINDEX *c); //File: Template.c; Line: 28
void templateMatching(char *cale_imagine, char *cale_sablon, double prag, int *nrDetectii, infoFereastra **detectii); //File: Template.c; Line: 42

//Functii pentru eliminarea non-maximelor
int overlap(infoFereastra a, infoFereastra b); //File: Template.c; Line: 161
int minim(int a, int b); //File: Template.c; Line: 170
int maxim(int a, int b); //File: Template.c; Line: 177
double overlapArea(infoFereastra a, infoFereastra b); //File: Template.c; Line: 184
void delete_nonmaxim(infoFereastra **detectii, int *nrDetectii); //File: Template.c; Line: 193

//Functie pentru desenarea unui chenar intr-o imagine
void contur(CULOAREINDEX ***M, CULOAREINDEX culoare, infoFereastra f); //File: Template.c; Line: 215

int cmpCorelatii(const void *a, const void *b); //File: Template.c; Line: 230

void colorareImg(char *cale_imagine); //File: Template.c; Line: 237

#endif // FUNCTIONS_H_
