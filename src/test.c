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
    int i, j;
	//debug("Merging. Begin: %d, Middle: %d, End: %d\n", begin, middle, end);
	for (int k = 0; k < vector->size0 + vector->size1; ++k) {
	//	debug("LHS[%d]: %d, RHS[%d]: %d\n", i, numbers[i], j, numbers[j]);
		if (i < vector->size0 &&
        (j >= vector->size1 || 
        vector->v0[i] < vector->v1[j])) {
			sorted[k] = vector->v0[i];
			i++;
		} else {
            sorted[k] = vector->v1[j];
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
    int a0[2] = {1, 5};
    int a1[2] = {4, 6};
    int sorted[4];
    malloc(sizeof(int) * 4);
    struct double_vector* dv = malloc(sizeof(struct double_vector));
    dv->v0 = a0;
    dv->v1 = a1;
    dv->size0 = 2;
    dv->size1 = 2;
    merge(dv, sorted);
    print_array(sorted, 4);
    return 0;
}
