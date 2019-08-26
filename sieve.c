/* **************************************************************
 *   Implementacao em C do Algoritmo Crivo de Eratóstenes       *
 *   duas implementacoes disponiveis:                           *
 *          1. paralela uttilizando o OpenMP                    *
 *          2. sequencial                                       *
 *   Para fins de otimizacao a implementatcao considera um      *
 *   array de bits. Ao fim, valores em 0 sao primos e           *
 *   valores em 1 nao sao primos.                               *
 *                                                              *
 *   Executado em ambiente cloud, utilizando a maquina          *
 *           n1-standard-8 da Google Cloud Plataform            *
 *   Instrucoes de execucao no arquivo README.md                *
 *                                                              *
 *  Autor: Fabio Alves Pereira                                  *
 ***************************************************************/
    
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
 
#define ISPRIME 0

/* Variaveis globais - entradas como parametro na execucao */
int INPUT_SIZE;
const char* TYPE_OUTPUT;
int NUM_THREAD;

/* ****************************************************** *
 *   Imprime instrucoes de execucao para o algoritmo      *
 *   Exemplifica, para cada parametro, a forma de         *
 *   entrada dos dados.                                   *
 *   @param const char* prog                              *
 *   @return void                                         *
 *********************************************************/
void print_usage(const char* prog);

/* ****************************************************** *
 *   Imprime os resultados de execucao do algoritmo       *
 *   @param char *array lista de numeros primos           *
 *   @param double time tempo de execucao em segundos     *
 *   @return void                                         *
 *********************************************************/
void print_output(char *array, double time);

/* ****************************************************** *
 *   Preenche as variaveis globais de acordo com os       * 
 *   parametros de entrada do algoritmo                   *
 *   @param int argc numero total de parametros           *
 *   @param const char** argv lista de parametros         *
 *   @return int                                          *
 *          0 - quantidade insuficiente de parametros     *
 *          1 - parametros corretos                       *
 *********************************************************/
int parse_args(int argc, const char** argv);

/* ****************************************************** *
 *   Retorna o bit do array baseado no index              * 
 *   @param char *array lista de numeros avaliados        *
 *   @param int index indice do array                     *
 *   @return char                                         *
 *********************************************************/
char get_bit(char *array, int index);

/* ****************************************************** *
 *   Inverte o valor do bit na posicao index do array     * 
 *   @param char *array lista de numeros avaliados        *
 *   @param int index indice do array                     *
 *   @return void                                         *
 *********************************************************/
void toggle_bit(char *array, int index);

/* ****************************************************** *
 *   Crivo de Eratóstenes  calcula todos os numeros       *
 *   primos ate um numero limite n.                       * 
 *   @param char *array lista de numeros avaliados        *
 *   @param int n quantidade limite de                    *
 *   numeros a avaliar.                                   *
 *   @param int sqrt_limit raiz quadrada                  *
 *   do numero limite de elementos a avaliar              *
 *   @return void                                         *
 *********************************************************/
void sieve_eratosthenes_bit(char *array, int n, int sqrt_limit);

/* ****************************************************** *
 *   Crivo de Eratóstenes  calcula todos os numeros       *
 *   primos ate um numero limite n.                       * 
 *                                                        *
 *   Implementacao paralela baseada no numero de threads  *
 *   passada como parametro para o algoritmo.             *
 *                                                        *
 *   @param char *array lista de numeros avaliados        *
 *   @param int n quantidade limite de                    *
 *   numeros a avaliar.                                   *
 *   @param int sqrt_limit raiz quadrada                  *
 *   do numero limite de elementos a avaliar              *
 *   @return void                                         *
 *********************************************************/
void parallel_sieve_eratosthenes_bit(char *array, int n, int sqrt_limit);

int main(int argc, const char** argv) {
    if(parse_args(argc, argv) == 1){
        /* Aloca array de char com bits já zerados. */
        char* array = calloc((INPUT_SIZE/8+(!!(INPUT_SIZE%8))), sizeof(char));
        struct timeval start, endt;
        float time_exec;

        int sqrt_limit = sqrt(INPUT_SIZE);

        /* Executa o crivo medindo o tempo de execucao por ciclos de clock */
        gettimeofday(&start,NULL);
            parallel_sieve_eratosthenes_bit(array, INPUT_SIZE, sqrt_limit);
        gettimeofday(&endt,NULL);
        time_exec = ((endt.tv_sec  - start.tv_sec) * 1000000u + endt.tv_usec - start.tv_usec) / 1.e6;
        
        print_output(array, time_exec);
        free(array);
    }
    return 0;
}

void parallel_sieve_eratosthenes_bit(char *array, int n, int sqrt_limit){ 
	int p, i;  
    #pragma omp parallel for schedule(dynamic) num_threads(NUM_THREAD) shared(array, sqrt_limit, n) private(p, i)
    for (p=2; p<=sqrt_limit; p++) {
        if (get_bit(array, p) == 0){
	       	for (i=p*p; i<=n; i += p){
                if(get_bit(array, i) == 0)
                    toggle_bit(array, i); 
            }
        }
    }
}

void sieve_eratosthenes_bit(char *array, int n, int sqrt_time){
    for (int p=2; p<=sqrt_time; p++) {
        if (get_bit(array, p) == 0){
            for (int i=p*p; i<=n; i += p){
                if(get_bit(array, i) == 0)
                    toggle_bit(array, i); 
            }
        }
    }
}

void toggle_bit(char *array, int index) {
    array[index / 8] ^= 1 << (index % 8);
}
 
char get_bit(char *array, int index) {
    return 1 & (array[index / 8] >> (index % 8));
}

void print_usage(const char* prog) {
    fprintf(stderr, "Você deve informar 3 parâmetros na linha de comando:\n");
    fprintf(stderr, "\t- N: (int) Limite superior [2, n] da lista de primos.\n");
    fprintf(stderr, "\t- S: (char) Forma de representar os dados de saida. a = all, t=time, l=lista de primos.\n");
    fprintf(stderr, "\t- P: (int) Numero de threads para a execucao.\n");

    fprintf(stderr, "\n");

    fprintf(stderr, "Exemplo:\n");
    fprintf(stderr, "\t$ %s 100 a 2\n", prog);

    fprintf(stderr, "\n");
}

void print_output(char *array, double time){
    char v;
    if(TYPE_OUTPUT[0] == 'a' || TYPE_OUTPUT[0] == 'l'){
        for (int p=2; p<=INPUT_SIZE; p++){
            v = get_bit(array, p);
            if (v == ISPRIME) 
                printf("%d ", p);
        }
        printf("\n");
    }
    if(TYPE_OUTPUT[0] == 'a' || TYPE_OUTPUT[0] == 't'){
        printf("%.6f\n", time);
    }
}

int parse_args(int argc, const char** argv) {
    if (argc < 4) {
        print_usage(argv[0]);
        return 0;
    } else {
        INPUT_SIZE = strtoull(argv[1], NULL, 10);
        TYPE_OUTPUT = argv[2];
        NUM_THREAD = atoi(argv[3]);
        return 1;
    }
}