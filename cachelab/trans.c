/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans_col(int M, int N, int A[N][M], int B[M][N]);
void trans_row(int M, int N, int A[N][M], int B[M][N]);
void trans_3232(int M, int N, int A[N][M], int B[M][N]);
void trans_6464(int M, int N, int A[N][M], int B[M][N]);
void trans_6167(int M, int N, int A[N][M], int B[M][N]);
/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    trans_6167(M, N, A, B);
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

char trans_col_desc[] = "Simple col-wise scan transpose";
void trans_col(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    for (j = 0; j < M; j++) {
        for (i = 0; i < N; i++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_row_desc[] = "Simple row-wise scan transpose";
void trans_row(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    
}

char trans_3232_desc[] = "32 * 32";
void trans_3232(int M, int N, int A[N][M], int B[M][N]) {
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < M; j += 8) {
            for (int k = 0; k < 8 && i + k < N; k++) {
                int a1 = A[i + k][j],
                    a2 = A[i + k][j + 1],
                    a3 = A[i + k][j + 2],
                    a4 = A[i + k][j + 3],
                    a5 = A[i + k][j + 4],
                    a6 = A[i + k][j + 5],
                    a7 = A[i + k][j + 6],
                    a8 = A[i + k][j + 7]
                    ;
                B[j][i + k] = a1;
                B[j + 1][i + k] = a2;
                B[j + 2][i + k] = a3;
                B[j + 3][i + k] = a4;
                B[j + 4][i + k] = a5;
                B[j + 5][i + k] = a6;
                B[j + 6][i + k] = a7;
                B[j + 7][i + k] = a8;
            }
        }
    }
}

char trans_6464_desc[] = "64 * 64";
void trans_6464(int M, int N, int A[N][M], int B[M][N]) {
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < M; j += 8) {
            /* divide 8 * 8 to 4 * 4 * 4 */
            /* copy block 1, 2 to B */
            for (int m = 0; m < 4; m++) {
                int a1 = A[i + m][j    ],
                    a2 = A[i + m][j + 1],
                    a3 = A[i + m][j + 2],
                    a4 = A[i + m][j + 3]
                    ;
                B[j    ][i + m] = a1;
                B[j + 1][i + m] = a2;
                B[j + 2][i + m] = a3;
                B[j + 3][i + m] = a4;
            }
            for (int m = 0; m < 4; m++) {
                int a1 = A[i + m][j + 4],
                    a2 = A[i + m][j + 5],
                    a3 = A[i + m][j + 6],
                    a4 = A[i + m][j + 7]
                    ;
                B[j    ][i + m + 4] = a1;
                B[j + 1][i + m + 4] = a2;
                B[j + 2][i + m + 4] = a3;
                B[j + 3][i + m + 4] = a4;
            }

            /* load A block 3 to B block 2 and in B, load block 2 to 3 */
            for (int m = 0; m < 4; m++) {
                /* store B block 2 */
                int b1 = B[j + m][i + 4],
                    b2 = B[j + m][i + 5],
                    b3 = B[j + m][i + 6],
                    b4 = B[j + m][i + 7]
                ;
                int a1 = A[i + 4][j + m],
                    a2 = A[i + 5][j + m],
                    a3 = A[i + 6][j + m],
                    a4 = A[i + 7][j + m]
                ;
                /* load A 3 to B 2 */
                B[j + m][i + 4] = a1;
                B[j + m][i + 5] = a2;
                B[j + m][i + 6] = a3;
                B[j + m][i + 7] = a4;
                
                B[j + 4 + m][i    ] = b1;
                B[j + 4 + m][i + 1] = b2;
                B[j + 4 + m][i + 2] = b3;
                B[j + 4 + m][i + 3] = b4;
            }
            /* load A block 4 to B block 4*/
            for (int m = 0; m < 4; m++) {
                int a1 = A[i + 4 + m][j + 4],
                    a2 = A[i + 4 + m][j + 5],
                    a3 = A[i + 4 + m][j + 6],
                    a4 = A[i + 4 + m][j + 7];
                B[j + 4][i + 4 + m] = a1;
                B[j + 5][i + 4 + m] = a2;
                B[j + 6][i + 4 + m] = a3;
                B[j + 7][i + 4 + m] = a4;
            }
        }
    }
}

char trans_6167_desc[] = "61 * 67";
void trans_6167(int M, int N, int A[N][M], int B[M][N]) {
    for (int i = 0; i + 8 < N; i += 8) {
        for (int j = 0; j + 8 < M; j += 8) {
            for (int k = 0; k < 8; k++) {
                int a1 = A[i + k][j],
                    a2 = A[i + k][j + 1],
                    a3 = A[i + k][j + 2],
                    a4 = A[i + k][j + 3],
                    a5 = A[i + k][j + 4],
                    a6 = A[i + k][j + 5],
                    a7 = A[i + k][j + 6],
                    a8 = A[i + k][j + 7]
                    ;
                B[j][i + k] = a1;
                B[j + 1][i + k] = a2;
                B[j + 2][i + k] = a3;
                B[j + 3][i + k] = a4;
                B[j + 4][i + k] = a5;
                B[j + 5][i + k] = a6;
                B[j + 6][i + k] = a7;
                B[j + 7][i + k] = a8;
            }
        }
    }
    for (int i = 0, j = 56; i < N; i++) {
        int a1 = A[i][j    ],
            a2 = A[i][j + 1],
            a3 = A[i][j + 2],
            a4 = A[i][j + 3],
            a5 = A[i][j + 4]
            ;
        B[j    ][i] = a1;
        B[j + 1][i] = a2;
        B[j + 2][i] = a3;
        B[j + 3][i] = a4;
        B[j + 4][i] = a5;
    }
    for (int j = 0; j < 56; j += 8) {
        for (int m = 64; m < 67; m++) {
            int a1 = A[m][j],
                a2 = A[m][j + 1],
                a3 = A[m][j + 2],
                a4 = A[m][j + 3],
                a5 = A[m][j + 4],
                a6 = A[m][j + 5],
                a7 = A[m][j + 6],
                a8 = A[m][j + 7]
            ;
            B[j][m] = a1;
            B[j + 1][m] = a2;
            B[j + 2][m] = a3;
            B[j + 3][m] = a4;
            B[j + 4][m] = a5;
            B[j + 5][m] = a6;
            B[j + 6][m] = a7;
            B[j + 7][m] = a8;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans_row, trans_row_desc);
    // registerTransFunction(trans_col, trans_col_desc);
    // registerTransFunction(trans_zigzag, trans_zigzag_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

