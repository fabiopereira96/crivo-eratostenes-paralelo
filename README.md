# Implementação Paralela do Crivo de Eratóstenes 

Projeto de implementação do algoritmo de busca de números primos, Crivo de Eratóstenes, utilizando
a biblioteca de paralelização OpenMP.

### Compile

Vá ate a pasta inicial do projeto e execute o makefile

```bash
  $ make
```

### Run

```bash
  $ ./ep1 N S P
```

- N: Número limite de elementos a serem avaliados.
- S: Tipo de exibição do resultado.
    a = all, t=time, l=lista de primos.
- P: Quantidade de threads para a execução paralela.