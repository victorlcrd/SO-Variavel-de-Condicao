# Exercício de SO: Produtor/Consumidor com variável de condição em C

Este repositório contém uma implementação do problema **Produtor/Consumidor** usando:

- linguagem C;
- biblioteca `pthread`;
- **mutex** para proteger a região crítica do buffer;
- duas **variáveis de condição**:
  - `cond_not_empty` (buffer não vazio);
  - `cond_not_full` (buffer não cheio).

## Arquivo principal

- `produtor_consumidor.c`

## Compilação

```bash
gcc produtor_consumidor.c -o produtor_consumidor -pthread
```

## Execução

```bash
./produtor_consumidor
```

## Descrição da sincronização utilizada

- O **produtor** bloqueia quando `buffer.count == BUFFER_SIZE`.
- O **consumidor** bloqueia quando `buffer.count == 0`.
- Ambos usam `while` ao redor de `pthread_cond_wait(...)` para reavaliar a condição após acordar.
- `pthread_cond_wait(...)` libera o mutex temporariamente enquanto a thread dorme e readquire o mutex antes de retornar.
- A variável de condição **não substitui** o mutex: ela só coordena espera/acordo; a proteção dos dados compartilhados continua sendo papel do mutex.

## O que os logs mostram

Durante a execução, o programa imprime mensagens quando:

- um item é produzido;
- um item é consumido;
- o produtor dorme por buffer cheio;
- o consumidor dorme por buffer vazio;
- uma thread acorda após `pthread_cond_signal(...)`.
