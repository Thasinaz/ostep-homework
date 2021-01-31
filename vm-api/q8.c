#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int *dat;
    size_t len;
} Stack;

Stack new_Stack() {
    Stack s = { NULL, 0 };
    return s;
}

int empty(Stack *s) {
    return s->len == 0;
}

void push(Stack *s, int i) {
    int *dat = s->dat;
    size_t len = s->len;

    dat = realloc(dat, (len + 1) * sizeof(int));
    dat[len++] = i;

    s->dat = dat;
    s->len = len;
}

int pop(Stack *s) {
    if (empty(s) == 1) {
        exit(1);
    }

    return s->dat[--s->len];
}

void free_Stack(Stack *s) {
    free(s->dat);

    s->dat = NULL;
    s->len = 0;
}

int main() {
    Stack s = new_Stack();

    for (int i = 0; i < 10; i++) {
        push(&s, i);
    }

    if (empty(&s) == 0) {
        printf("%d", pop(&s));
    }
    for (int i = 0; empty(&s) == 0; i++) {
        printf(" %d", pop(&s));
    }
    putchar('\n');

    free_Stack(&s);
    return 0;
}
