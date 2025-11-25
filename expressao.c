// expressao.c - Implementa avaliação e conversão de expressões matemáticas
// Implementa: getFormaInFixa (pública), getValorPosFixa (pública)
// Internamente: static getFormaPosFixa (infixa -> posfixa)
// Compilar com: gcc expressao.c main.c -lm -o calculadora.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
Utilitários internos
*/

static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    char *r = malloc(strlen(s) + 1);
    if (r) strcpy(r, s);
    return r;
}

static void replace_comma_with_dot(char *s) {
    for (; *s; ++s) if (*s == ',') *s = '.';
}

/* reconhece nomes válidos de funções (aceita variantes) */
static int is_function_name(const char *s) {
    if (!s) return 0;
    return (!strcasecmp(s,"sen") || !strcasecmp(s,"sin") ||
            !strcasecmp(s,"cos") ||
            !strcasecmp(s,"tg")  || !strcasecmp(s,"tan") ||
            !strcasecmp(s,"log") ||
            !strcasecmp(s,"raiz")|| !strcasecmp(s,"sqrt"));
}

static void normalize_func_name(const char *in, char *out, size_t outsz) {
    if (!strcasecmp(in,"sin") || !strcasecmp(in,"sen")) strncpy(out,"sen",outsz);
    else if (!strcasecmp(in,"cos")) strncpy(out,"cos",outsz);
    else if (!strcasecmp(in,"tg") || !strcasecmp(in,"tan")) strncpy(out,"tg",outsz);
    else if (!strcasecmp(in,"log")) strncpy(out,"log",outsz);
    else if (!strcasecmp(in,"sqrt") || !strcasecmp(in,"raiz")) strncpy(out,"raiz",outsz);
    else {
        size_t i;
        for (i=0; i+1<outsz && in[i]; ++i) out[i] = (char)tolower(in[i]);
        out[i] = '\0';
    }
}

/* 
AVALIAÇÃO PÓS-FIXA 
getValorPosFixa: avalia expressão em notação pós-fixa.
Tokens devem estar separados por espaços. Números com . ou , são aceitos.
Retorna float (ou NAN em erro).
*/
float getValorPosFixa(char *StrPosFixa) {
    if (!StrPosFixa) return NAN;
    char *copy = strdup_safe(StrPosFixa);
    if (!copy) return NAN;
    replace_comma_with_dot(copy);

    char *save = NULL;
    char *tok = strtok_r(copy, " \t\r\n", &save);

    float stack[512]; int sp = 0;

    while (tok) {
        /* número */
        char *end; float v = strtof(tok, &end);
        if (*end == '\0' && end != tok) {
            stack[sp++] = v;
            tok = strtok_r(NULL, " \t\r\n", &save);
            continue;
        }

        /* função unária */
        if (is_function_name(tok)) {
            if (sp < 1) { free(copy); return NAN; }
            float a = stack[--sp]; float r = NAN;
            if (!strcasecmp(tok,"sen") || !strcasecmp(tok,"sin")) r = sinf(a * (M_PI/180.0f));
            else if (!strcasecmp(tok,"cos")) r = cosf(a * (M_PI/180.0f));
            else if (!strcasecmp(tok,"tg") || !strcasecmp(tok,"tan")) r = tanf(a * (M_PI/180.0f));
            else if (!strcasecmp(tok,"log")) r = (a <= 0.0f ? NAN : log10f(a));
            else if (!strcasecmp(tok,"raiz") || !strcasecmp(tok,"sqrt")) r = (a < 0.0f ? NAN : sqrtf(a));
            stack[sp++] = r;
            tok = strtok_r(NULL, " \t\r\n", &save);
            continue;
        }

        /* operador binário */
        if (strlen(tok) == 1 && strchr("+-*/%^", tok[0])) {
            if (sp < 2) { free(copy); return NAN; }
            float b = stack[--sp]; float a = stack[--sp]; float r = NAN;
            switch (tok[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/': r = (b == 0.0f ? NAN : a / b); break;
                case '%': r = (b == 0.0f ? NAN : fmodf(a, b)); break;
                case '^': r = powf(a, b); break;
            }
            stack[sp++] = r;
            tok = strtok_r(NULL, " \t\r\n", &save);
            continue;
        }

        /* token desconhecido */
        free(copy); return NAN;
    }

    float result = (sp == 1) ? stack[0] : NAN;
    free(copy);
    return result;
}

/*
POS-FIXA -> INFIXA (parênteses mínimos)
*/

typedef struct { char *s; int prec; } ExprItem;

static int prec_op(char op) {
    switch (op) { case '+': case '-': return 1; case '*': case '/': case '%': return 2; case '^': return 3; }
    return 0;
}
static int is_right_assoc(char op) { return op == '^'; }

/* 
getFormaInFixa: converte pós-fixa (tokens espaços) -> infixa (malloc). 
*/

char * getFormaInFixa(char *StrPosFixa) {
    if (!StrPosFixa) return NULL;
    char *copy = strdup_safe(StrPosFixa);
    if (!copy) return NULL;
    replace_comma_with_dot(copy);

    ExprItem stack[512]; int sp = 0;
    char *save = NULL; char *tok = strtok_r(copy, " \t\r\n", &save);

    while (tok) {
        char *end; strtof(tok, &end);
        if (*end == '\0' && end != tok) {
            stack[sp].s = strdup_safe(tok); stack[sp].prec = 5; sp++;
            tok = strtok_r(NULL, " \t\r\n", &save); continue;
        }
        if (is_function_name(tok)) {
            if (sp < 1) { free(copy); return NULL; }
            ExprItem a = stack[--sp];
            char fname[32]; normalize_func_name(tok, fname, sizeof(fname));
            size_t len = strlen(fname) + strlen(a.s) + 4;
            char *buf = malloc(len + 1);
            if (!buf) { free(a.s); free(copy); return NULL; }
            snprintf(buf, len+1, "%s(%s)", fname, a.s);
            free(a.s);
            stack[sp].s = buf; stack[sp].prec = 4; sp++;
            tok = strtok_r(NULL, " \t\r\n", &save); continue;
        }
        if (strlen(tok)==1 && strchr("+-*/%^", tok[0])) {
            if (sp < 2) { free(copy); return NULL; }
            char op = tok[0]; ExprItem r = stack[--sp]; ExprItem l = stack[--sp];
            int p = prec_op(op);
            int needL = (l.prec < p) || (l.prec == p && is_right_assoc(op));
            int needR = (r.prec < p) || (r.prec == p && !is_right_assoc(op));
            size_t len = strlen(l.s) + strlen(r.s) + 8;
            char *buf = malloc(len + 1);
            if (!buf) { free(l.s); free(r.s); free(copy); return NULL; }
            char *ptr = buf;
            if (needL) *ptr++ = '(';
            strcpy(ptr, l.s); ptr += strlen(l.s);
            if (needL) *ptr++ = ')';
            *ptr++ = op;
            if (needR) *ptr++ = '(';
            strcpy(ptr, r.s); ptr += strlen(r.s);
            if (needR) *ptr++ = ')';
            *ptr = '\0';
            free(l.s); free(r.s);
            stack[sp].s = buf; stack[sp].prec = p; sp++;
            tok = strtok_r(NULL, " \t\r\n", &save); continue;
        }
        free(copy); return NULL;
    }

    if (sp != 1) { free(copy); return NULL; }
    char *res = stack[0].s;
    free(copy);
    return res; 
}

/*
INFIXA -> POSFIXA (SHUNTING-YARD) - função INTERNA
*/

/* 
A função fica interna (static) conforme enunciado. Saída usa formato "operando função" para unárias. 
*/

static int is_operator_char(char c) { return strchr("+-*/%^", c) != NULL; }
static int prec_char(char c) {
    switch (c) { case '+': case '-': return 1; case '*': case '/': case '%': return 2; case '^': return 3; }
    return 0;
}
static int right_assoc_char(char c) { return c == '^'; }

/* 
getFormaPosFixa: static internal (infixa -> posfixa). Tokens separados por espaço.
Formato de funções na saída: operando function   (ex: 45 sen)
*/
static char * getFormaPosFixa(char *StrInfixa) {
    if (!StrInfixa) return NULL;
    char *in = strdup_safe(StrInfixa);
    if (!in) return NULL;
    replace_comma_with_dot(in);
    size_t len = strlen(in);

    char *out = malloc(len*3 + 64);
    if (!out) { free(in); return NULL; }
    out[0] = '\0';

    char *opstack[512];
    int top = -1;

    for (size_t i = 0; i < len; ) {
        char c = in[i];
        if (isspace((unsigned char)c)) { i++; continue; }

        /* número */
        if (isdigit((unsigned char)c) || c == '.') {
            size_t j = i;
            while (j < len && (isdigit((unsigned char)in[j]) || in[j] == '.')) j++;
            strncat(out, in + i, j - i);
            strcat(out, " ");
            i = j; continue;
        }

        /* identificador / função */
        if (isalpha((unsigned char)c)) {
            size_t j = i;
            while (j < len && isalpha((unsigned char)in[j])) j++;
            size_t toklen = j - i;
            char *ident = malloc(toklen + 1);
            strncpy(ident, in + i, toklen);
            ident[toklen] = '\0';
            opstack[++top] = ident; // push function name
            i = j; continue;
        }

        /* parêntese esquerdo */
        if (c == '(') { opstack[++top] = strdup_safe("("); i++; continue; }

        /* parêntese direito */
        if (c == ')') {
            while (top >= 0 && strcmp(opstack[top], "(") != 0) {
                strcat(out, opstack[top]); strcat(out, " "); free(opstack[top--]);
            }
            if (top >= 0 && strcmp(opstack[top], "(") == 0) { free(opstack[top--]); }
            else { while (top >= 0) free(opstack[top--]); free(in); free(out); return NULL; }
            if (top >= 0 && isalpha((unsigned char)opstack[top][0])) {
                strcat(out, opstack[top]); strcat(out, " "); free(opstack[top--]);
            }
            i++; continue;
        }

        /* operador */
        if (is_operator_char(c)) {
            char cur[2] = {c, '\0'};
            while (top >= 0 && is_operator_char(opstack[top][0])) {
                char topc = opstack[top][0];
                int p1 = prec_char(topc), p2 = prec_char(c);
                if ((p1 > p2) || (p1 == p2 && !right_assoc_char(c))) {
                    strcat(out, opstack[top]); strcat(out, " "); free(opstack[top--]);
                } else break;
            }
            opstack[++top] = strdup_safe(cur);
            i++; continue;
        }

        /* invalido */
        while (top >= 0) free(opstack[top--]);
        free(in); free(out); return NULL;
    }

    while (top >= 0) {
        if (strcmp(opstack[top], "(") == 0) { free(opstack[top--]); continue; }
        strcat(out, opstack[top]); strcat(out, " "); free(opstack[top--]);
    }

    size_t L = strlen(out);
    while (L > 0 && isspace((unsigned char)out[L-1])) out[--L] = '\0';

    free(in);
    return strdup_safe(out);
}

