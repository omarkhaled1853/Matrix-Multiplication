#include <stdio.h>
#include <stdlib.h>
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

// allocate space for the arr in struct Matrix
void allocate_arr(struct Matrix *matrix){
    matrix->arr = (int **) calloc(matrix->rows, sizeof(int *));
    for (int i = 0; i < matrix->rows; i++) {
        matrix->arr[i] = (int *) calloc(matrix->cols, sizeof(int));
    }
}

// printing arr in struct Matrix
void print_matrix(struct Matrix *matrix){
    for(int i = 0; i < matrix->rows; i++){
        for(int j = 0; j < matrix->cols; j++){
            printf("%d ", matrix->arr[i][j]);
        }
        printf("\n");
    }
}

// check validation of matrix multiplication
int check_not_valid_mult(struct Matrix *a, struct Matrix *b){
    if (a->cols != b->rows) {
        printf("Columns of first matrix must be equal to rows of second matrix\n");
        for (int i = 0; i < a->rows; i++)
            free(a->arr[i]);
        free(a->arr);
        free(a);
        for (int i = 0; i < b->rows; i++)
            free(b->arr[i]);
        free(b->arr);
        free(b);
        return 6;
    }
}

// check validation of arguments
int check_not_valid_arg(int *argc){
    if(argc != 1 && argc != 4){
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

// write output matrix based on each method
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

int main(int argc, char *argv[]) {
    
    // check_not_valid_arg(argc);

    // struct Matrix *a = malloc(sizeof(struct Matrix));
    // struct Matrix *b = malloc(sizeof(struct Matrix));

    // // No arguments
    // if(argv[1] == NULL){
    //     if(read_matrix("a", a) == -1){
    //         return 2;
    //     }
    //     if(read_matrix("b", b) == -1){
    //         return 3;
    //     }
    //     output = "c";
    // }
    // // Custom arrguments
    // else {
    //     if(read_matrix(argv[1], a) == -1){
    //         return 4;
    //     }
    //     if(read_matrix(argv[2], b) == -1){
    //         return 5;
    //     }
    //     output = argv[3];
    // }
    
    // check_not_valid_mult(a, b);
    
    // print_matrix(a);
    // printf("\n");
    // print_matrix(b);

    output = "c";
    struct Matrix *c = malloc(sizeof(struct Matrix));
    c->rows = 4;
    c->cols = 4;
    allocate_arr(c);
    for(int i = 0; i < c->rows; i++){
        for(int j = 0; j < c->cols; j++){
            c->arr[i][j] = 5;
        }
    }

    write_matrix(0, c);
    write_matrix(1, c);
    write_matrix(2, c);

    return 0;
}
