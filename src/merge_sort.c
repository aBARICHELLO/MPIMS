#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#ifndef DEBUG
#define DEBUG 2
#endif

#ifndef NELEMENTS
#define NELEMENTS 100
#endif

#ifndef MAXVAL
#define MAXVAL 255
#endif

int st;
int number_of_processes;
int rank;
int minimum_vector_size;
int parent;
int current_depth;
int chunk_size;

int original_depth = 0;

// MPI_TAG = 0, parametros
// MPI_TAG = 1, parte do vetor
const int TAG_PARAMS = 0;
const int TAG_VECTOR = 1;

struct vector {
    int* vector;
    int size;
};

struct double_vector {
    struct vector vector0;
    struct vector vector1;
};

// Recebendo parametros do pai
void receive_params() {
    // Inicializando vetor a ser recebido
    int buffer[3];
    // Recebe parametros da arvore binaria
    MPI_Recv(&buffer, 3, MPI_INT, MPI_ANY_SOURCE, TAG_PARAMS, MPI_COMM_WORLD, &st);
    // Atribuindo parametros
    parent = buffer[0];
    current_depth = buffer[1];
    chunk_size = buffer[2];
    original_depth = current_depth;
}

// Enviar vetor para o filho, junto com os parametros da árvore binária
void send_vector(struct vector v) {
    // Cria vetor com os parametros para o filho
    int buffer[3] = {rank, current_depth + 1, v.size};
    // Calcula destino
    int destiny = rank + pow(2, current_depth);
    // Manda parametros
    MPI_Send(&buffer, 3, MPI_INT, destiny, TAG_PARAMS, MPI_COMM_WORLD);
    // Manda vetor
    MPI_Send(&v, v.size, MPI_INT, destiny, TAG_VECTOR, MPI_COMM_WORLD);
    // Aumenta profundidade do processo
    current_depth++;
}

struct vector receive_vector_from_child(int size) {
    int source = rank + pow(2, current_depth);
    MPI_Recv(&buffer, size, MPI_INT, source, TAG_VECTOR, MPI_COMM_WORLD, &st);
    current_depth--;
}

struct vector receive_vector() {
    int buffer[chunk_size];
    MPI_Recv(&buffer, chunk_size, MPI_INT, parent, TAG_VECTOR, MPI_COMM_WORLD, &st);
    struct vector vector;
    vector->vector = buffer;
    vector->size = chunk_size;
    return vector;
}

// Divide o vetor ao meio retornando uma struct double vector
struct double_vector divide_vector(struct vector v) {
    struct double_vector dv;
    int half = ceil(v.size / 2);
    int first_v[half];
    int second_v[v.size - half];

    struct vector v1;
    struct vector v2;

    for (int i = 0; i < half; i++) {
        first_v[i] = v[i];
    }

    for (int i = half; i < v.size; i++) {
        second_v[i] = v[i];
    }

    v0.vector = first_v;
    v1.vector = second_v;
    v0.size = half;
    v1.size = v.size - half;

    dv.vector0 = v0;
    dv.vector1 = v1;
    return dv;
}

// More info on: http://en.cppreference.com/w/c/language/variadic
void debug(const char* msg, ...) {
    if (DEBUG > 2) {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}


// Orderly merges two int arrays (numbers[begin..middle] and numbers[middle..end]) into one (sorted).
// \retval: merged array -> sorted
void merge(struct double_vector* to_be_merged) {
    int i, j;
    int sorted[to_be_merged->vector0.size + to_be_merged->vector1.size];
    for (int k = 0; k < to_be_merged->vector0.size + to_be_merged->vector1.size; ++k) {
        if (i < to_be_merged->vector0.size && (j >= to_be_merged->vector1.size ||
            to_be_merged->vector0.vector[i] < to_be_merged->vector1.vector[j])) {
            sorted->vector[k] = to_be_merged->vector0.vector[i];
            i++;
        } else {
            sorted->vector[k] = to_be_merged->vector1.vector[j];
            j++;
        }
    }

    return sorted;
}

// Ordenando sequencialmente a parte minima do vetor
struct vector sequential_merge_sort(struct vector unsorted) {
    struct double_vector vectors = divide_vector(unsorted);
    struct vector v0 = sequential_merge_sort(vectors.vector0);
    struct vector v1 = sequential_merge_sort(vectors.vector1);

    vectors.vector0 = v0;
    vectors.vector1 = v1;
    return merge(vectors);
}

// First Merge Sort call
void merge_sort(int* unsorted, int size, int* sorted) {
    // Receber parametros pai, altura e tamanho do vetor
    receive_params();
    // Calculando tamanho minimo para parar o split dos vetores
    minimum_vector_size = ceil(size / number_of_processes);

    struct vector unsorted_vector = malloc(sizeof(struct vector));
    unsorted_vector->vector = unsorted;
    unsorted_vector->size = size;

    // Se rank for zero, utiliza argumento (unsorted)
    if (rank != 0) {
        // Rank 0 é o único que utiliza o numbers parametro, o resto utiliza o recebido pelo MPI
        unsorted_vector = receive_vector();
    }

    // Merge sort loop
    while (unsorted_vector.size > minimum_vector_size) {
        // Divide vetor em dois
        struct double_vector vectors = divide_vector(unsorted_vector);
        // Envia parte vetor para processo "da direita" na árvore binária
        send_vector(vectors.vector1);
        // Fica com parte para si e repete
        unsorted_vector = vectors.vector0;
    }

    // Após reduzir vetor em partes, realizar etapa sequencial
    struct vector my_sorted_vector = sequential_merge_sort(unsorted_vector)

    while(current_depth >= original_depth) {
        struct vector sorted_sub_vector = receive_from_child();
        struct double_vector vectors;
        merge(vectors);
    }

    send_vector_to_parent(my_sorted_vector);
}

void print_array(int* array, int size) {
    printf("Printing Array:\n");
    for (int i = 0; i < size; ++i) {
        printf("%d. ", array[i]);
    }
    printf("\n");
}

void populate_array(int* array, int size, int max) {
    int m = max + 1;
    for (int i = 0; i < size; ++i) {
        array[i] = rand() % m;
    }
}

int main(int argc, char** argv) {
    int seed, max_val;
    int* sortable;
    int* sorted;
    size_t arr_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    // // TODO: Arrumar casos de debug
    // // Basic MERGE unit test        
    // if (DEBUG > 1) {
    //     int * a = (int*)malloc(8 * sizeof(int));
    //     a[0] = 1; a[1] = 3; a[2] = 4; a[3] = 7;
    //     a[4] = 0; a[5] = 2; a[6] = 5; a[7] = 6;
    //     int * values = (int*)malloc(8 * sizeof(int));
    //     merge(a, 0, 4, 8, values);
    //     free(a);
    //     print_array(values, 8);
    //     free(values);
    //     return 2;
    // }

    // // Basic MERGE-SORT unit test
    // if (DEBUG > 0) {
    //     int* a = (int*)malloc(8 * sizeof(int));
    //     int* b = (int*)malloc(8 * sizeof(int));
    //     a[0] = 7; a[1] = 6; a[2] = 5; a[3] = 4;
    //     a[4] = 3; a[5] = 2; a[6] = 1; a[7] = 0;

    //     b = memcpy(b, a, 8 * sizeof(int));
    //     merge_sort(a, 8, b);
    //     print_array(b, 8);

    //     free(a);
    //     free(b);

    //     a = (int*)malloc(9 * sizeof(int));
    //     b = (int*)malloc(9 * sizeof(int));
    //     a[0] = 3; a[1] = 2; a[2] = 1;
    //     a[3] = 10; a[4] = 11; a[5] = 12;
    //     a[6] = 0; a[7] = 1; a[8] = 1;

    //     b = memcpy(b, a, 9*sizeof(int));
    //     print_array(b, 9);
    //     merge_sort(a, 9, b);
    //     print_array(b, 9);

    //     free(a);
    //     free(b);
    //     printf("\n");
    //     return 1;
    // }

    switch (argc) {
        case 1:
            seed = time(NULL);
            arr_size = NELEMENTS;
            max_val = MAXVAL;
            break;
        case 2:
            seed = atoi(argv[1]);
            arr_size = NELEMENTS;
            max_val = MAXVAL;
            break;
        case 3:
            seed = atoi(argv[1]);
            arr_size = atoi(argv[2]);
            max_val = MAXVAL;
            break;
        case 4:
            seed = atoi(argv[1]);
            arr_size = atoi(argv[2]);
            max_val = atoi(argv[3]);
            break;
        default:
            printf("Too many arguments\n");
            break;
    }

    srand(seed);
    sortable = malloc(arr_size * sizeof(int));
    sorted = malloc(arr_size * sizeof(int));

    populate_array(sortable, arr_size, max_val);
    sorted = memcpy(sorted, sortable, arr_size * sizeof(int));

    print_array(sortable, arr_size);
    merge_sort(sortable, arr_size, sorted);
    print_array(sorted, arr_size);

    free(sortable);
    free(sorted);

    MPI_Finalize();

    return 0;
}
