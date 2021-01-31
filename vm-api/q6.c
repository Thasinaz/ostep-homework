#include <stdio.h>
#include <stdlib.h>

int main() {
    int *data = (int *) malloc(100 * sizeof(int));
    *data = 100;
    printf("%d ", *data);
    free(data);
    printf("%d\n", *data);
    return 0;
}
