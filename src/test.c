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
    //printf("\nPrinting Array:\n");
    for (int i = 0; i < size; ++i) {
        printf("%d. ", array[i]);
    }
   // printf("\n");
}

struct vector merge(struct double_vector to_be_merged) {
    int i = 0;
    int j = 0;
    int merged_size = to_be_merged.vector0.size + to_be_merged.vector1.size;
    int sorted_vector[merged_size];

    struct vector sorted = {
        .size = merged_size,
        .vector = sorted_vector 
    };

    for (int k = 0; k < to_be_merged.vector0.size + to_be_merged.vector1.size; ++k) {
        if (i < to_be_merged.vector0.size && (j >= to_be_merged.vector1.size || to_be_merged.vector0.vector[i] < to_be_merged.vector1.vector[j])) {
            // printf("Vetor sorted posição %d recebe %d pois %d < %d\n", k, to_be_merged.vector0.vector[i], to_be_merged.vector0.vector[i], to_be_merged.vector1.vector[j]);
            printf("k = %d, i = %d, j = %d, elemento = %d\n", k, i, j, to_be_merged.vector0.vector[i]);
            sorted.vector[k] = to_be_merged.vector0.vector[i];
            i++;
        } else {
            // printf("Vetor sorted posicao %d recebe %d pois %d > %d\n", k, to_be_merged.vector1.vector[j], to_be_merged.vector0.vector[i], to_be_merged.vector1.vector[j]);
            printf("k = %d, i = %d, j = %d, elemento = %d\n", k, i, j, to_be_merged.vector1.vector[j]);
            sorted.vector[k] = to_be_merged.vector1.vector[j];
            j++;
        }
    }
    return sorted;
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
  
    return dv;
}

// Teste função merge
void merge_test() {
    int a0[3] = {1, 5, 7};
    int a1[4] = {4, 6, 10, 23};

    struct double_vector dv;
    dv.vector0.vector = a0;
    dv.vector1.vector = a1;
    dv.vector0.size = 3;
    dv.vector1.size = 4;
    struct vector sorted = merge(dv);
    print_array(sorted.vector, sorted.size);
    printf("\n");
}

void  divide_vector_test() {
    int a0[5] = {4, 6, 10, 23, 25};
    struct vector vector = { .vector = a0, .size = 5};
    struct double_vector divided_vector = divide_vector(vector);
    print_array(divided_vector.vector0.vector, divided_vector.vector0.size); 
    printf("\n");
    print_array(divided_vector.vector1.vector, divided_vector.vector1.size);    
   

}


int main() {
    printf("Testing merge\n");
    merge_test();
    printf("Testing division\n");
    divide_vector_test();    
}
