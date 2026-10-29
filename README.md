# Calculadora de Expressões em C

Módulo acadêmico para avaliar expressões pós-fixas e convertê-las para a forma infixa. Utiliza pilhas e funções da biblioteca matemática de C.

## Recursos

- Operadores `+`, `-`, `*`, `/`, `%` e `^`.
- Seno, cosseno e tangente em graus.
- Logaritmo decimal e raiz quadrada.
- Tokens separados por espaços na entrada pós-fixa.

## Exemplo

A expressão pós-fixa `3 12 4 + *` corresponde a `3*(12+4)` e tem resultado `48`.

## Compilar e executar

Com GCC em ambiente que ofereça `strtok_r` e `strcasecmp` (por exemplo, Linux ou WSL), execute na raiz:

```bash
gcc -std=gnu11 -I. expressao.c examples/demo.c -lm -o calculadora
./calculadora
```

O exemplo em [examples/demo.c](examples/demo.c) fornece o ponto de entrada; `expressao.c` é o módulo de implementação.

## API pública

| Função | Responsabilidade |
| --- | --- |
| `getValorPosFixa` | Avalia a expressão; retorna `NAN` em erros reconhecidos |
| `getFormaInFixa` | Retorna uma string alocada com a expressão infixa; o chamador libera com `free` |

Consulte [expressao.h](expressao.h) para as declarações e [expressao.c](expressao.c) para a implementação. A conversão de infixa para pós-fixa é uma função interna, não exposta no cabeçalho.

## Contexto

Projeto de estudo. A implementação utiliza pilhas de tamanho fixo; entradas muito extensas e validação completa de expressões são pontos para evolução.

## Limites e testes

As funções rejeitam entradas com mais de 511 caracteres, evitando ultrapassar as pilhas de tamanho fixo. Entradas inválidas retornam `NAN` ou `NULL`, conforme a função.

```bash
gcc -std=gnu11 -Wall -Wextra -I. expressao.c tests/test_expressao.c -lm -o teste
./teste
```

Os testes cobrem avaliação, conversão, divisão por zero, expressão incompleta e entrada longa. A conversão interna de infixa para pós-fixa continua preservada como material do exercício.

## Guia do código

Consulte [GUIA_CODIGO.md](GUIA_CODIGO.md) para localizar os módulos, entender as convenções e estudar as alterações.
