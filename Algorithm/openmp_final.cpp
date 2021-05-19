#include <iostream>
#include <string>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <bits/stdc++.h>
#include <sstream>
#include <fstream>
#include <string>
#include <omp.h>

/*
matrix format:
 A  b
-c  0 

input filename = dimension of A 
e.g.    if A is 10x100, filename should be 10x100.txt
        here,   nL = 10
                nC = 100

compile command     - $ g++ -std=c++11 openmp_parallel.cpp -fopenmp
execution command   - $ ./a.out <filename> 
*/

using namespace std;
#define CLK CLOCK_MONOTONIC

/* user-defined structures to save the index and the value in pairs */
class Max_Updated
{
public:
    double value = 0;
    int index = 0;
};

class Min_Updated
{
public:
    double value = HUGE_VAL;
    int index = 0;
};

#pragma omp declare reduction(find_min:Min_Updated \
                              : omp_out = omp_in.value < omp_out.value ? omp_in : omp_out)
#pragma omp declare reduction(find_max:Max_Updated \
                              : omp_out = omp_in.value > omp_out.value ? omp_in : omp_out)

/* to measure algorithm and end-to-end time */
struct timespec My_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;

    if ((end.tv_nsec - start.tv_nsec) < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}

/* 
 * to retrieve the dimensions of the data matrix from filename 
 * nL = number of rows
 * nC = number of columns 
 */
void get_dimension(char **argv, int &nL, int &nC)
{
    int shape[2] = {0, 0}, i = 0;
    char *pch = strtok(argv[1], "x/_");
    while (pch != NULL)
    {
        if (i == 1)
        {
            shape[0] = atoi(pch);
        }
        if (i == 2)
        {
            shape[1] = atoi(pch);
        }
        pch = strtok(NULL, "x/_");
        i++;
    }
    nL = shape[0] + 1;
    nC = shape[0] + shape[1] + 1;
}

int main(int argc, char **argv)
{
    struct timespec start_e2e, end_e2e, start_alg, end_alg;

    /* should start before anything else */
    clock_gettime(CLK, &start_e2e);

    /* check if enough command-line arguments are taken in */
    if (argc < 3)
    {
        printf("Usage: %s n p \n", argv[0]);
        return -1;
    }

    char *N = argv[1];     /* size of input array */
    int P = atoi(argv[2]); /* number of processors*/
    char problem_name[] = "simplex";
    char approach_name[] = "parallel";

    FILE *outputFile;

    char outputFileName[50];
    sprintf(outputFileName, "output/%s_%s_%s_%s_output.txt", problem_name, approach_name, argv[1], argv[2]);

    /* declaration of variables */
    int num_threads, constraint_num, column_num, ni = 0, chunk_size = 1, nL, nC;
    int count = 0, count_neg = 0;
    ifstream file(argv[1]);
    get_dimension(argv, nL, nC);
    vector<vector<double>> standard_simplex_tableau(nL, vector<double>(nC, 0));

    /* initialization - setting dimensions and maximum number of iterations */
    int NMAX = (nL + nC)*100;
    constraint_num = nL;
    column_num = nC;

    /* initialization - getting data from file to save in table */
    string line;
    int l = 0, i = 0;
    while (getline(file, line))
    {
        istringstream iss(line);
        double value;
        i = 0;
        while (iss >> value && i < nC)
        {
            standard_simplex_tableau[l][i] = value;
            i++;
        }
        l++;
    }

    constraint_num--;
    column_num--;

    num_threads = atoi(argv[2]);
    omp_set_num_threads(num_threads);

    ni = 0;

    Max_Updated max;
    Min_Updated min;

    int flag = 1;

    if (argc == 3)
        chunk_size = 100;
    else
        chunk_size = atoi(argv[3]);

    clock_gettime(CLK, &start_alg); /* Start the algo timer */

/* start parallel */
#pragma omp parallel default(none) shared(min, max, chunk_size, num_threads, count, standard_simplex_tableau, count_neg, column_num, constraint_num, ni, NMAX, flag)
    {
        double simplex_pivot1, simplex_pivot2, simplex_pivot3;
        int i, j;

/* finds maximum value from reduced cost row */
#pragma omp for schedule(guided, chunk_size) reduction(find_max \
                                                       : max)
        for (j = 0; j <= column_num; j++)
            if (standard_simplex_tableau[constraint_num][j] < 0.0 && max.value < (-standard_simplex_tableau[constraint_num][j]))
            {
                max.value = -standard_simplex_tableau[constraint_num][j];
                max.index = j;
            }
#pragma omp single nowait
        max.value = 0;

        do
        {

/* pivot row selection
 ** count the number of positive values
 ** if all < 0, then solution is unbounded
 ** else find the MinRatio
*/
#pragma omp for reduction(+         \
                          : count), \
    reduction(find_min                \
              : min) schedule(guided, chunk_size)
            for (i = 0; i < constraint_num; i++)
            {
                if (standard_simplex_tableau[i][max.index] > 0.0)
                {
                    simplex_pivot1 = standard_simplex_tableau[i][column_num] / standard_simplex_tableau[i][max.index];
                    if (min.value > simplex_pivot1)
                    {
                        min.value = simplex_pivot1;
                        min.index = i;
                    }
                }
                else
                    count++;
            }

/* check for unbounded solution */
#pragma omp single nowait
            {
                if (count == constraint_num)
                {
                    printf("Solution not found\n");
                    flag = 0;
                }
                else
                    count = 0;
                count_neg = 0;
            }

            simplex_pivot1 = standard_simplex_tableau[min.index][max.index];
            simplex_pivot3 = -standard_simplex_tableau[constraint_num][max.index];

/* convert pivot element to 1 and rest to 0 */
#pragma omp barrier
#pragma omp for
            for (j = 0; j <= (column_num); j++)
            {
                standard_simplex_tableau[min.index][j] = standard_simplex_tableau[min.index][j] / simplex_pivot1;
            }

/* perform row operations to modify all other rows except the pivot row */
#pragma omp for nowait
            for (i = 0; i < constraint_num; i++)
            {
                if (i != min.index)
                {
                    simplex_pivot2 = -standard_simplex_tableau[i][max.index];

#pragma omp simd simdlen(4)
                    for (j = 0; j <= column_num; j++)
                    {
                        standard_simplex_tableau[i][j] = (simplex_pivot2 * standard_simplex_tableau[min.index][j]) + standard_simplex_tableau[i][j];
                    }
                }
            }

/* updating reduced costs and finding maximum value for next iteration */
#pragma omp barrier
#pragma omp for reduction(+             \
                          : count_neg), \
    reduction(find_max                    \
              : max) schedule(guided, chunk_size)
            for (j = 0; j <= column_num; j++)
            {
                standard_simplex_tableau[constraint_num][j] = (simplex_pivot3 * standard_simplex_tableau[min.index][j]) + standard_simplex_tableau[constraint_num][j];
                if (j < column_num && standard_simplex_tableau[constraint_num][j] < 0.0)
                {
                    count_neg++;
                    if (max.value < (-standard_simplex_tableau[constraint_num][j]))
                    {
                        max.value = -standard_simplex_tableau[constraint_num][j];
                        max.index = j;
                    }
                }
            }

/* incrementing counter and resetting values for next iteration */
#pragma omp single
            {
                ni++;
                max.value = 0.0;
                min.value = HUGE_VAL;
                printf("%d\n", ni);
            }

        } while (count_neg && ni < NMAX && flag == 1);
    }

    clock_gettime(CLK, &end_alg); /* End the algo timer */

    double ONE_SECOND_IN_NANOSECONDS = 1000000000;
    struct timespec time;

    clock_gettime(CLK, &end_e2e); /* End the e2e timer */

    time = My_diff(start_e2e, end_e2e);
    double e2e = (time.tv_sec + (double)time.tv_nsec / ONE_SECOND_IN_NANOSECONDS);

    time = My_diff(start_alg, end_alg);
    double alg = (time.tv_sec + (double)time.tv_nsec / ONE_SECOND_IN_NANOSECONDS);

    printf("%s,%s,%s,%d,%f,%f,%f,%f,%d\n", problem_name, approach_name, N, P, e2e, e2e * ONE_SECOND_IN_NANOSECONDS, alg, alg * ONE_SECOND_IN_NANOSECONDS, flag);

    // printf("%f %f ", processTime / ni, processTime);
    // printf("%d %f \n", ni, standard_simplex_tableau[constraint_num][column_num]);
    // printf("flag = %d\n", flag);

    standard_simplex_tableau.clear();
    standard_simplex_tableau.shrink_to_fit();

    return 0;
}