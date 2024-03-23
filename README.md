# Matrix Multiplication (Multi-Threading)
## Overview
* This project implements three different approaches to matrix multiplication, each utilizing multi-threading with varying granularity.
* The goal is to compare the performance and resource utilization of these approaches based on the number of threads created and the execution time taken.

## Approaches
## Approach 1: One Thread per Matrix
* In this approach, each input matrix (A and B) is multiplied using one thread. The output matrix C is computed without multi-threading.

## Approach 2: One Thread per Row
* In this approach, each row of the output matrix C is computed using a separate thread. This results in creating as many threads as there are rows in the output matrix.

## Approach 3: One Thread per Element
* In this approach, each element of the output matrix C is computed using a separate thread. This results in creating as many threads as there are elements in the output matrix.

## Comparison Criteria
* Number of Threads Created: Compare the total number of threads created for each approach.
* Execution Time Taken: Measure the execution time of each approach and compare their performance.
  
## Excution
* program is executed as: ./matMultp Mat1 Mat2 MatOut, where Mat1 and Mat2 are the names of the text files to read the first and second matrices.
* MatOut is the prefix of the text files names to write the output matrices (each file represents one method).
* If the user does not enter this information, the default is a.txt and b.txt, for input matrixes A and B, and c for output matrices (of all three methods) C.

## License
* This project is licensed under the MIT License.
