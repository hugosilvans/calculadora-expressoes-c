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
