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

// Atributos do processo MPI
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

void print_array(int* array, int size) {
    if (rank != 0) return;
    printf("Printing Array:\n");
    for (int i = 0; i < size; ++i) {
        printf("%d. ", array[i]);
    }
    printf("\n");
}

// Recebendo parametros do pai
void receive_params() {
    if (rank == 0) {
        return;
    } 
    
    // Inicializando vetor a ser recebido
    int buffer[3];
    // Recebe parametros da arvore binaria
    MPI_Recv(&buffer, 3, MPI_INT, MPI_ANY_SOURCE, TAG_PARAMS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // Atribuindo parametros
    parent = buffer[0];
    current_depth = buffer[1];
    chunk_size = buffer[2];
    original_depth = current_depth;
}

// Enviar vetor para o filho, junto com os parametros da árvore binária
void send_vector_to_child(struct vector v) {
    // Cria vetor com os parametros para o filho
    int buffer[3] = {rank, current_depth + 1, v.size};
    // Calcula destino
    int destiny = rank + pow(2, current_depth);
    // Manda parametros
    MPI_Send(&buffer, 3, MPI_INT, destiny, TAG_PARAMS, MPI_COMM_WORLD);
    // Manda vetor
    MPI_Send(v.vector, v.size, MPI_INT, destiny, TAG_VECTOR, MPI_COMM_WORLD);
    // Apagando vetor enviado da memoria
    free(v.vector);
    // Aumenta profundidade do processo
    current_depth++;
}

void send_vector_to_parent(struct vector vector) {
    MPI_Send(&vector.size, 1, MPI_INT, parent, TAG_PARAMS, MPI_COMM_WORLD);
    MPI_Send(vector.vector, vector.size, MPI_INT, parent, TAG_VECTOR, MPI_COMM_WORLD);
    free(vector.vector);
}

// Recebe vetor do nodo filho da árvore binária
// Nodo a enviar dados na árvore binária = rank + 2^profundidade
struct vector receive_vector_from_child() {
    current_depth--;
    int source = rank + pow(2, current_depth);
    int size;
    MPI_Recv(&size, 1, MPI_INT, source, TAG_PARAMS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int* buffer = malloc(sizeof(int) * size);
    MPI_Recv(buffer, size, MPI_INT, source, TAG_VECTOR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    struct vector v = { .vector = buffer, .size = size };
    
    return v;
}

// Funcao para recebe vetor desordenado do pai
struct vector receive_vector_from_parent() {
    int* buffer = malloc(sizeof(int) * chunk_size);
    MPI_Recv(buffer, chunk_size, MPI_INT, parent, TAG_VECTOR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    struct vector v = { .vector = buffer, .size = chunk_size };

    return v;
}

// Divide o vetor ao meio retornando uma struct double vector
struct double_vector divide_vector(struct vector v) {
    int half = v.size / 2;
    int* first_v = malloc(sizeof(int) * half );
    int* second_v = malloc(sizeof(int) * (v.size - half));

    for (int i = 0; i <= half; i++) {
        first_v[i] = v.vector[i];
    }
    
    for (int i = half, j = 0; i < v.size; i++, j++) {
        second_v[j] = v.vector[i];
    }

    struct vector v0 = { .vector = first_v, .size = half };
    struct vector v1 = { .vector = second_v, .size = v.size - half };
    struct double_vector dv = { v0, v1 }; 
    free(v.vector);
    
    return dv;
}

void debug(const char* msg, ...) {
    if (DEBUG > 2) {
        va_list args;
        va_start(args, msg);
        vprintf(msg, args);
        va_end(args);
    }
}

struct vector merge(struct double_vector to_be_merged) {
    int i = 0;
    int j = 0;
    int merged_size = to_be_merged.vector0.size + to_be_merged.vector1.size;
    int* sorted_vector = malloc(sizeof(int) * merged_size);

    struct vector sorted = { .size = merged_size, .vector = sorted_vector };

    for (int k = 0; k < to_be_merged.vector0.size + to_be_merged.vector1.size; ++k) {
        if (i < to_be_merged.vector0.size && (j >= to_be_merged.vector1.size || to_be_merged.vector0.vector[i] < to_be_merged.vector1.vector[j])) {
            sorted.vector[k] = to_be_merged.vector0.vector[i];
            i++;
        } else {
            sorted.vector[k] = to_be_merged.vector1.vector[j];
            j++;
        }
    }

    free(to_be_merged.vector0.vector);
    free(to_be_merged.vector1.vector);
    
    return sorted;
}

// Ordenando sequencialmente a parte minima do vetor
struct vector sequential_merge_sort(struct vector unsorted) {
    struct double_vector vectors = divide_vector(unsorted);
    
    if (vectors.vector0.size >= 1 && vectors.vector1.size >= 1) {
        struct vector v0 = sequential_merge_sort(vectors.vector0);
        struct vector v1 = sequential_merge_sort(vectors.vector1);
        vectors.vector0 = v0;
        vectors.vector1 = v1;
    }
    return merge(vectors);
}

void merge_sort(int* unsorted, int size, int* sorted) {
    // Receber parametros pai, altura e tamanho do vetor
    receive_params();
    // Calculando tamanho minimo para parar o split dos vetores
    minimum_vector_size = (size + number_of_processes - 1) / number_of_processes;
    struct vector unsorted_vector = { .size = size, .vector = unsorted };

    // Se rank for zero, utiliza argumento (unsorted)
    if (rank != 0) {
        // Rank 0 é o único que utiliza o numbers parametro, o resto utiliza o recebido pelo MPI
        unsorted_vector = receive_vector_from_parent();
    }
    // Merge sort loop
    while (unsorted_vector.size > minimum_vector_size) {
        if (number_of_processes % 2 == 1 && number_of_processes <= rank + pow(2, current_depth)) {
            break;
        }
        // Divide vetor em dois
        struct double_vector vectors = divide_vector(unsorted_vector);
        // Envia parte vetor para processo "da direita" na árvore binária
        send_vector_to_child(vectors.vector1);
        // Fica com parte para si e repete
        unsorted_vector = vectors.vector0;
    }

    // Após reduzir vetor em partes, realizar etapa sequencial
    struct vector my_sorted_vector = sequential_merge_sort(unsorted_vector);

    // Fazendo caminho de volta para receber vetores
    while (current_depth > original_depth) {
        // Recebe vetor ordenado do filho
        struct vector sorted_sub_vector = receive_vector_from_child();
        //criando estrutura auxiliar
        struct double_vector vectors = { .vector0 = my_sorted_vector, .vector1 = sorted_sub_vector };
        // Junta vetores ordenados
        my_sorted_vector = merge(vectors);
    }

    // Se for filho, deve enviar vetor ordenado para o pai
    // Se for o processo 0, deve copiar vetor ordenado para retornar
    if (rank != 0) {
        send_vector_to_parent(my_sorted_vector);
    } else {
        for(size_t i = 0; i < size; i++) {
            sorted[i] = my_sorted_vector.vector[i];
        }
    }

}

void populate_array(int* array, int size, int max) {
    int m = max + 1;
    for (int i = 0; i < size; ++i) {
        array[i] = rand() % m;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

    int seed, max_val;
    int* sortable;
    int* sorted;
    size_t arr_size;

    // Basic MERGE-SORT unit test
    if (DEBUG > 0) {
        int* a = (int*)malloc(8 * sizeof(int));
        int* b = (int*)malloc(8 * sizeof(int));
        a[0] = 7; a[1] = 6; a[2] = 5; a[3] = 4;
        a[4] = 3; a[5] = 2; a[6] = 1; a[7] = 0;

        b = memcpy(b, a, 8 * sizeof(int));
        merge_sort(a, 8, b);
        print_array(b, 8);

        free(a);
        free(b);

        a = (int*)malloc(9 * sizeof(int));
        b = (int*)malloc(9 * sizeof(int));
        a[0] = 3; a[1] = 2; a[2] = 1;
        a[3] = 10; a[4] = 11; a[5] = 12;
        a[6] = 0; a[7] = 1; a[8] = 1;

        b = memcpy(b, a, 9*sizeof(int));
        print_array(b, 9);
        merge_sort(a, 9, b);
        print_array(b, 9);

        free(a);
        free(b);
        printf("\n");
        MPI_Finalize();
        return 0;
    }

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
}
