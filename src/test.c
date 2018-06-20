#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


struct double_vector {
    int* v0;
    int* v1;
    int size0;
    int size1;
};

void merge(struct double_vector* vector, int* sorted) {
    int j = 0;
    int k = 0;
    for (int i = 0; i < vector->size0 + vector->size1; ++i) {
        if (vector->v0[j] < vector->v1[k]) {
            sorted[i] = vector->v0[j++];
        } else {
            sorted[i] = vector->v1[k++];
        }
    }
}

void print_array(int* array, int size) {
    printf("Printing Array:\n");
    for (int i = 0; i < size; ++i) {
        printf("%d. ", array[i]);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    int a0[2] = {1, 5};
    int a1[1] = {4};
    int sorted[3];
    malloc(sizeof(int) * 3);
    struct double_vector* dv = malloc(sizeof(struct double_vector));
    dv->v0 = a0;
    dv->v1 = a1;
    dv->size0 = 2;
    dv->size1 = 1;
    merge(dv, sorted);
    print_array(sorted, 3);
    return 0;
}
