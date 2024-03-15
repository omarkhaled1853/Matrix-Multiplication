#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include <sys/time.h>

#define MAX_SIZE 20

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




void print_matrix(struct Matrix *matrix){
    for(int i = 0; i < matrix->rows; i++){
        for(int j = 0; j < matrix->cols; j++){
            printf("%d ", matrix->arr[i][j]);
        }
        printf("\n");
    }
}

void allocate_arr(struct Matrix *matrix){
    matrix->arr = (int **) calloc(matrix->rows, sizeof(int *));
    for (int i = 0; i < matrix->rows; i++) {
        matrix->arr[i] = (int *) calloc(matrix->cols, sizeof(int));
    }
}


void read_matrix(struct Matrix *matrix){
    scanf("%d %d", &matrix->rows, &matrix->cols);
    allocate_arr(matrix);
    for(int i = 0; i < matrix->rows; i++){
        for(int j = 0; j < matrix->cols; j++){
            scanf("%d", &matrix->arr[i][j]);
        }
    }
}


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


int main(){
    struct Matrix *a = malloc(sizeof(struct Matrix));
    struct Matrix *b = malloc(sizeof(struct Matrix));
    read_matrix(a);
    read_matrix(b);
    // print_matrix(a);
    // print_matrix(b);
    // initialize_mult_per_matrix(a, b);
    return 0;
}