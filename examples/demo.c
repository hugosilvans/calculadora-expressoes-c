#include <stdio.h>
#include <stdlib.h>
#include "expressao.h"

int main(void) {
    char entrada[] = "3 12 4 + *";
    char *infixa = getFormaInFixa(entrada);
    if (infixa == NULL) {
        fprintf(stderr, "Nao foi possivel converter a expressao.\n");
        return 1;
    }
    printf("Pos-fixa: %s\nInfixa: %s\nValor: %.2f\n",
           entrada, infixa, getValorPosFixa(entrada));
    free(infixa);
    return 0;
}
