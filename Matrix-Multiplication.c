#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>
#include<sys/time.h>
#include <string.h>

FILE *file;
char *output;

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

// free heap
void free_Matrix(struct Matrix *matrix){
    for (int i = 0; i < matrix->rows; i++)
        free(matrix->arr[i]);
    free(matrix->arr);
    free(matrix);
}


// check validation of matrix multiplication
int check_not_valid_mult(struct Matrix *a, struct Matrix *b){
    if (a->cols != b->rows) {
        printf("Columns of first matrix must be equal to rows of second matrix\n");
        free_Matrix(a);
        free_Matrix(b);
        return 6;
    }
}

// check validation of arguments
int check_not_valid_arg(int *argc){
    if(*argc != 1 && *argc != 4){
        perror("Error in arguments\n");
        return 1;
    }
}

// read the matrix from the text file
int read_matrix(const char name[], struct Matrix *matrix) {
    // build the name of the file
    char *filename = calloc(5 + strlen(name), sizeof(char));
    strcpy(filename, name);
    strcat(filename, ".txt");

    // open file to read
    file = fopen(filename, "r");

    // error in reading the file or file does not exist
    if (file == NULL) {
        printf("Error reading the file: %s\n", filename);
        return -1;
    }

    int rows, cols;
    // read row and col
    fscanf(file, "row=%d col=%d", &rows, &cols);

    matrix->rows = rows;
    matrix->cols = cols;
    
    allocate_arr(matrix);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fscanf(file, "%d", &matrix->arr[i][j]) != 1) {
                printf("Error reading the file: %s\n", filename);
                return -1;
            }
        }
    }


    free(filename);
    fclose(file);
    return 0;
}

// write output matrix based on each methode
void write_matrix(int type, struct Matrix *c) {
    char filename[256];
    strcpy(filename, output);
    switch (type) {
        case 0:
            strcat(filename, "_per_matrix");
            break;
        case 1:
            strcat(filename, "_per_row");
            break;
        case 2:
            strcat(filename, "_per_element");
            break;
    }
    strcat(filename, ".txt");
    FILE *out = fopen(filename, "w");
    switch (type) {
        case 0:
            fprintf(out, "Method: A thread per matrix\n");
            break;
        case 1:
            fprintf(out, "Method: A thread per row\n");
            break;
        case 2:
            fprintf(out, "Method: A thread per element\n");
            break;
    }
    fprintf(out, "row=%d col=%d\n", c->rows, c->cols);
    for (int i = 0; i < c->rows; i++) {
        for (int j = 0; j < c->cols; j++) {
            fprintf(out, "%d ", c->arr[i][j]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
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
    write_matrix(0, c);
    free_Matrix(c);
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
    write_matrix(1, c);
    free_Matrix(c);
}

// operation of multiplication per element
void* mult_per_element(void* args){
    struct Multiplication *per_element = (struct Multiplication *) args;
    int sum = 0;
    for (int k = 0; k < per_element->b->rows; k++) {
        sum += per_element->a->arr[per_element->row][k] * per_element->b->arr[k][per_element->col];
    }
    per_element->c->arr[per_element->row][per_element->col] = sum;
    free(per_element);
    pthread_exit(NULL);
    return NULL;
}

// initialization of threads and allocation
int initialize_mult_per_element(struct Matrix *a, struct Matrix *b){
    pthread_t th[a->rows][b->cols];
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
            if(pthread_create(&th[i][j], NULL, mult_per_element, (void *)per_element)){
                perror("Faild creating thread\n");
                return 1;
            }
        }
    }
    for (int i = 0; i < a->rows; i++) {
        for (int j = 0; j < b->cols; j++) {
            pthread_join(th[i][j], NULL);
        }
    }
    write_matrix(2, c);
    free_Matrix(c);
}

void *thread_per_element(void *args) {
    struct Multiplication *per_element;
    per_element = (struct Multiplication *) args;
    int sum = 0;
    for (int k = 0; k < per_element->b->rows; k++) {
        sum += per_element->a->arr[per_element->row][k] * per_element->b->arr[k][per_element->col];
    }
    per_element->c->arr[per_element->row][per_element->col] = sum;
    free(per_element);
    pthread_exit(NULL);
}

// main function of the method of per element
void thread_per_element_main(struct Matrix *A, struct Matrix *B) {
    int rows = A->rows, cols = B->cols;
    pthread_t threads[rows][cols];
    int **res = (int **) calloc(A->rows, sizeof(int *));
    for (int i = 0; i < A->rows; i++) {
        res[i] = (int *) calloc(B->cols, sizeof(int));
    }
    struct Matrix *C = malloc(sizeof(struct Matrix));
    C->rows = A->rows; C->cols = B->cols; C->arr = res;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            struct Multiplication *per_element = malloc(sizeof(struct Multiplication));
            per_element->row = i; per_element->col = j;
            per_element->a = A; per_element->b = B; per_element->c = C;
            if (pthread_create(&threads[i][j], NULL, thread_per_element, (void *) per_element)) {
                printf("Can not create a thread\n");
                exit(1);
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }
    write_matrix(2, C);
    for (int i = 0; i < C->rows; i++)
        free(C->arr[i]);
    free(C->arr);
    free(C);
}


int main(int argc, char *argv[]){

    check_not_valid_arg(&argc);

    struct Matrix *a = malloc(sizeof(struct Matrix));
    struct Matrix *b = malloc(sizeof(struct Matrix));

    // No arguments
    if(argv[1] == NULL){
        if(read_matrix("a", a) == -1){
            return 2;
        }
        if(read_matrix("b", b) == -1){
            return 3;
        }
        output = "c";
    }
    // Custom arrguments
    else {
        if(read_matrix(argv[1], a) == -1){
            return 4;
        }
        if(read_matrix(argv[2], b) == -1){
            return 5;
        }
        output = argv[3];
    }
    
    check_not_valid_mult(a, b);
    
    struct timeval stop, start;

    gettimeofday(&start, NULL); //start checking time for first method
    initialize_mult_per_matrix(a, b);
    gettimeofday(&stop, NULL); //end checking time for first method

    printf("First Methode:\n");
    printf("Number of threads: %d\n", 1);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("========================================================================\n");

    gettimeofday(&start, NULL); //start checking time for second method
    initialize_mult_per_row(a, b);
    gettimeofday(&stop, NULL); //end checking time for second method

    printf("Second Methode:\n");
    printf("Number of threads: %d\n", a->rows);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("========================================================================\n");


    gettimeofday(&start, NULL); //start checking time for third method
    initialize_mult_per_element(a, b);
    gettimeofday(&stop, NULL); //end checking time for third method

    printf("Third Methode:\n");
    printf("Number of threads: %d\n", a->rows * b->cols);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("========================================================================\n");

    free_Matrix(a);
    free_Matrix(b);
    return 0;
}