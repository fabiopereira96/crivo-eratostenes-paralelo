#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
 
#define ISPRIME 0

/* Variaveis globais - entradas como parametro na execucao */
unsigned long long INPUT_SIZE;
const char* TYPE_OUTPUT;
int NUM_THREAD;


void print_usage(const char* prog);
void print_output(char *array, double time, int i, int k);
int parse_args(int argc, const char** argv);

char get_bit(char *array, int index);
void toggle_bit(char *array, int index);

void sieve_eratosthenes_bit(char *array, unsigned long long n, unsigned long long sqrt_limit);
void parallel_sieve_eratosthenes_bit(char *array, unsigned long long n, unsigned long long sqrt_limit);

int main(int argc, const char** argv) {
    if(parse_args(argc, argv) == 1){
        int numMax = NUM_THREAD;
        for(int k=1; k<=numMax; k++){
            NUM_THREAD = k;
            for(int i=0; i<10; i++){
                /* Aloca array de char com bits já zerados. */
                char* array = calloc((INPUT_SIZE/8+(!!(INPUT_SIZE%8))), sizeof(char));
                struct timeval start, endt;
                float time_exec;

                unsigned long long sqrt_limit = sqrt(INPUT_SIZE);

                /* Executa o crivo medindo o tempo de execucao por ciclos de clock */
                gettimeofday(&start,NULL);
                    parallel_sieve_eratosthenes_bit(array, INPUT_SIZE, sqrt_limit);
                gettimeofday(&endt,NULL);
                time_exec = ((endt.tv_sec  - start.tv_sec) * 1000000u + endt.tv_usec - start.tv_usec) / 1.e6;
                
                print_output(array, time_exec, i, k);
                free(array);
            }
        }
    }
    return 0;
}

void parallel_sieve_eratosthenes_bit(char *array, unsigned long long n, unsigned long long sqrt_limit){ 
	unsigned long long p, i;  
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

void sieve_eratosthenes_bit(char *array, unsigned long long n, unsigned long long sqrt_time){
    for (unsigned long long p=2; p<=sqrt_time; p++) {
        if (get_bit(array, p) == 0){
            for (unsigned long long i=p*p; i<=n; i += p){
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
    fprintf(stderr, "\t- N: (unsigned long long) Limite superior [2, n] da lista de primos.\n");
    fprintf(stderr, "\t- S: (char) Forma de representar os dados de saida. a = all, t=time, l=lista de primos.\n");
    fprintf(stderr, "\t- P: (int) Numero de threads para a execucao.\n");

    fprintf(stderr, "\n");

    fprintf(stderr, "Exemplo:\n");
    fprintf(stderr, "\t$ %s 100 a 2\n", prog);

    fprintf(stderr, "\n");
}

void print_output(char *array, double time, int i, int k){
    char v;
    if(TYPE_OUTPUT[0] == 'a' || TYPE_OUTPUT[0] == 'l'){
        for (unsigned long long p=2; p<=INPUT_SIZE; p++){
            v = get_bit(array, p);
            if (v == ISPRIME) 
                printf("%llu ", p);
        }
        printf("\n");
    }
    if(TYPE_OUTPUT[0] == 'a' || TYPE_OUTPUT[0] == 't'){
        printf("i: %d, threads: %d, time: %.6f\n", i, k, time);
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
