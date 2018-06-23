#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>


struct vector {
    int* vector;
    int size;
};

struct double_vector {
    struct vector vector0;
    struct vector vector1;
};

void print_array(int* array, int size) {
    printf("\nPrinting Array:\n");
    for (int i = 0; i < size; ++i) {
        printf("%d. ", array[i]);
    }
    printf("\n");
}

struct vector* merge(struct double_vector to_be_merged) {
    int i;
    int j;
    printf("começando merge\n");

    struct vector* sorted = malloc(sizeof(struct vector));
    sorted->vector = malloc(sizeof(int*) * 4);

    printf("começando merge %d\n", to_be_merged.vector0.size + to_be_merged.vector1.size);
    sorted->size = to_be_merged.vector0.size + to_be_merged.vector1.size;
    printf("filhodaputa%d", sorted->size);
    int sorted_vector[4];
    sorted->vector = sorted_vector;

    for (int k = 0; k < to_be_merged.vector0.size + to_be_merged.vector1.size; ++k) {
        if (i < to_be_merged.vector0.size && (j >= to_be_merged.vector1.size || to_be_merged.vector0.vector[i] < to_be_merged.vector1.vector[j])) {
            printf("Vetor sorted posição %d recebe %d pois %d < %d\n", k, to_be_merged.vector0.vector[i], to_be_merged.vector0.vector[i], to_be_merged.vector1.vector[j]);
            sorted->vector[k] = to_be_merged.vector0.vector[i];
            i++;
        } else {
            printf("Vetor sorted posicao %d recebe %d pois %d > %d\n", k, to_be_merged.vector1.vector[j], to_be_merged.vector0.vector[i], to_be_merged.vector1.vector[j]);
            sorted->vector[k] = to_be_merged.vector1.vector[j];
            j++;
        }
    }
    print_array(sorted->vector, sorted->size);
    return sorted;
}

int main(int argc, char** argv) {
    // Teste função merge
    int a0[2] = {1, 5};
    int a1[2] = {4, 6};

    struct double_vector dv;
    dv.vector0.vector = a0;
    dv.vector1.vector = a1;
    dv.vector0.size = 2;
    dv.vector1.size = 2;
    struct vector* sorted = merge(dv);
    print_array(sorted->vector, sorted->size);
}
