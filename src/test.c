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

void merge(struct double_vector* to_be_merged, struct vector* sorted) {
    int i, j;
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
}

void print_array(int* array, int size) {
    printf("Printing Array:\n");
    for (int i = 0; i < size; ++i) {
        printf("%d. ", array[i]);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    printf("aa");
    int a0[2] = {1, 5};
    int a1[2] = {4, 6};

    struct double_vector* dv = malloc(sizeof(struct double_vector));
    struct vector* sorted = malloc(sizeof(struct vector));
    int a3[4];
    sorted->vector = a3;
    sorted->size = 4;

    dv->vector0.vector = a0;
    dv->vector1.vector = a1;
    dv->vector0.size = 2;
    dv->vector1.size = 2;
    merge(dv, sorted);
    print_array(sorted->vector, sorted->size);
    return 0;
}
