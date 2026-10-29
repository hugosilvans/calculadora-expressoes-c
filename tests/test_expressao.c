#include "expressao.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char expressao[] = "3 12 4 + *";
    assert(fabsf(getValorPosFixa(expressao) - 48) < 0.001f);
    char *infixa = getFormaInFixa(expressao);
    assert(infixa && strcmp(infixa, "3*(12+4)") == 0);
    free(infixa);
    char invalida[] = "3 +";
    assert(isnan(getValorPosFixa(invalida)));
    assert(getFormaInFixa(invalida) == NULL);
    char divisao[] = "1 0 /";
    assert(isnan(getValorPosFixa(divisao)));
    char longa[2048];
    memset(longa, '1', sizeof longa - 1);
    longa[sizeof longa - 1] = '\0';
    assert(isnan(getValorPosFixa(longa)));
    assert(getFormaInFixa(longa) == NULL);
    for (int i = 0; i < 100; i++) {
        char incompleta[] = "1 2 desconhecido";
        assert(getFormaInFixa(incompleta) == NULL);
    }
    return 0;
}
