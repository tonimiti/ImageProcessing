#include <stdio.h>
#include <stdlib.h>
#include "Functions.h"

int main()
{
    char cale_imagine[30], cale_imagine_criptata[30], cale_imagine_decriptata[30], cale_cheie[30], cale_imagine_template[30];
    FILE *fin = fopen("DateProiect.txt", "rt");
    if(fin == NULL)
    {
        printf("FILE ERROR (main.c\\main - DateProiect.txt)\n");
        return 0;
    }
    fscanf(fin, "%s%s%s%s%s", cale_imagine, cale_imagine_criptata, cale_imagine_decriptata, cale_cheie, cale_imagine_template);
    fclose(fin);

    unsigned char *header;
    header = (unsigned char*)malloc(54*sizeof(unsigned char));
    if(header == NULL)
    {
        printf("MEMORY ALLOCATION FAILED (main.c\\main - header)\n");
        return 0;
    }

    CULOAREINDEX *L;
    readBMP(cale_imagine, header, &L);
    test_chi2(cale_imagine);
    encryptBMP(cale_imagine, cale_imagine_criptata, cale_cheie, header, L);
    test_chi2(cale_imagine_criptata);
    readBMP(cale_imagine_criptata, header, &L);
    decryptBMP(cale_imagine_criptata, cale_imagine_decriptata, cale_cheie, header, L);

    printf("Rulare template matching\n");
    colorareImg(cale_imagine_template);

    free(header); free(L);
    return 0;
}
