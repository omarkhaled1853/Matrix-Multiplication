#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<sys/time.h>

struct Matrix
{
    int rows;
    int cols;
    int **arr;
};

struct Multiplication
{
    int row;
    int col;
    struct Matrix *a;
    struct Matrix *b;
    struct Matrix *c;
};


// printing arr in struct Matrix
void print_matrix(struct Matrix *matrix){
    for(int i = 0; i < matrix->rows; i++){
        for(int j = 0; j < matrix->cols; j++){
            printf("%d ", matrix->arr[i][j]);
        }
        printf("\n");
    }
}

// allocate space for the arr in struct Matrix
void allocate_arr(struct Matrix *matrix){
    matrix->arr = (int **) calloc(matrix->rows, sizeof(int *));
    for (int i = 0; i < matrix->rows; i++) {
        matrix->arr[i] = (int *) calloc(matrix->cols, sizeof(int));
    }
}

// read input
void read_matrix(struct Matrix *matrix){
    scanf("%d %d", &matrix->rows, &matrix->cols);
    allocate_arr(matrix);
    for(int i = 0; i < matrix->rows; i++){
        for(int j = 0; j < matrix->cols; j++){
            scanf("%d", &matrix->arr[i][j]);
        }
    }
}

// operation of multiplication per matrix
void* mult_per_matrix(void* args){
    struct Multiplication *per_matrix = (struct Multiplication *) args;
    for(int i = 0; i < per_matrix->a->rows; i++){
        for(int j = 0; j < per_matrix->b->cols; j++){
            per_matrix->c->arr[i][j] = 0;
            for(int k = 0; k < per_matrix->a->cols; k++){
                per_matrix->c->arr[i][j] += per_matrix->a->arr[i][k] * per_matrix->b->arr[k][j];
            }
        }
    }
    free(per_matrix);
    pthread_exit(NULL);
    return NULL;
}

// initialization of threads and allocation
int initialize_mult_per_matrix(struct Matrix *a, struct Matrix *b){
    pthread_t th;
    struct Matrix *c = malloc(sizeof(struct Matrix));
    c->rows = a->rows;
    c->cols = b->cols;
    allocate_arr(c);
    struct Multiplication *per_matrix = malloc(sizeof(struct Multiplication));
    per_matrix->row = 0;
    per_matrix->col = 0;
    per_matrix->a = a;
    per_matrix->b = b;
    per_matrix->c = c;
    if(pthread_create(&th, NULL, mult_per_matrix, (void *)per_matrix)){
        perror("Faild creating thread\n");
        return 1;
    }
    if(pthread_join(th, NULL)){
        perror("Faild joining thread\n");
        return 2;
    }
    print_matrix(c);
    for (int i = 0; i < c->rows; i++)
        free(c->arr[i]);
    free(c->arr);
    free(c);
}

// operation of multiplication per row
void* mult_per_row(void* args){
    struct Multiplication *per_row = (struct Multiplication *) args;
    int i = per_row->row;
    for(int j = 0; j < per_row->b->cols; j++){
        per_row->c->arr[i][j] = 0;
        for(int k = 0; k < per_row->a->cols; k++){
            per_row->c->arr[i][j] += per_row->a->arr[i][k] * per_row->b->arr[k][j];
        }
    }
    free(per_row);
    pthread_exit(NULL);
    return NULL;
} 

// initialization of threads and allocation
int initialize_mult_per_row(struct Matrix *a, struct Matrix *b){
    int n = a->rows, i;
    pthread_t th[n];
    struct Matrix *c = malloc(sizeof(struct Matrix));
    c->rows = a->rows;
    c->cols = b->cols;
    allocate_arr(c);
    for(i = 0; i < n; i++){
        struct Multiplication *per_row = malloc(sizeof(struct Multiplication));
        per_row->row = i;
        per_row->col = 0;
        per_row->a = a;
        per_row->b = b;
        per_row->c = c;
        if(pthread_create(&th[i], NULL, mult_per_row, (void *)per_row)){
            perror("Faild creating thread\n");
            return 1;
        }
    }
    for(i = 0; i < n; i++){
        if(pthread_join(th[i], NULL)){
            perror("Faild joining thread\n");
            return 2;
        }
    }
    print_matrix(c);
    for (int i = 0; i < c->rows; i++)
        free(c->arr[i]);
    free(c->arr);
    free(c);
}

// operation of multiplication per element
void* mult_per_element(void* args){
    struct Multiplication *per_element = (struct Multiplication *) args;
    int i = per_element->row, j = per_element->col;
    per_element->c->arr[i][j] = 0;
    for(int k = 0; k < per_element->b->cols; k++){
        per_element->c->arr[i][j] += per_element->a->arr[i][k] * per_element->b->arr[k][j];
    }
    free(per_element);
    pthread_exit(NULL);
    return NULL;
}

// initialization of threads and allocation
int initialize_mult_per_element(struct Matrix *a, struct Matrix *b){
    int n = a->rows * b->cols, k = 0;
    pthread_t th[n];
    struct Matrix *c = malloc(sizeof(struct Matrix));
    c->rows = a->rows;
    c->cols = b->cols;
    allocate_arr(c);
    for(int i = 0; i < a->rows; i++){
        for(int j = 0; j < b->cols; j++){
            struct Multiplication *per_element = malloc(sizeof(struct Multiplication));
            per_element->row = i;
            per_element->col = j;
            per_element->a = a;
            per_element->b = b;
            per_element->c = c;
            if(pthread_create(&th[k], NULL, mult_per_element, (void *)per_element)){
                perror("Faild creating thread\n");
                return 1;
            }
            k++;
        }
    }
    for(k = 0; k < n; k++){
        if(pthread_join(th[k], NULL)){
            perror("Faild joining thread\n");
            return 2;
        }
    }
    print_matrix(c);
    for (int i = 0; i < c->rows; i++)
        free(c->arr[i]);
    free(c->arr);
    free(c);
}


int main(){
    struct Matrix *a = malloc(sizeof(struct Matrix));
    struct Matrix *b = malloc(sizeof(struct Matrix));
    read_matrix(a);
    read_matrix(b);
    initialize_mult_per_matrix(a, b);
    initialize_mult_per_row(a, b);
    initialize_mult_per_element(a, b);
    return 0;
}