#include<bits/stdc++.h>

#include <omp.h>



/*
matrix format:
 A  b
-c  0 
filename should be shape of A i.e., with A of dimension 56x137 keep file name 56x137.txt
nL = 57 
nC = 195
to compile  $g++ -std=c++11 serial.cpp
to execute  $./a.out 56x137.txt 
*/
using namespace std;
class Max_Updated {
    public :
        double val = 0;
        int index = 0;
};

class Min_Updated {
    public:
        double val = HUGE_VAL;
        int index = 0;
};

// measure the time between end and start 
struct timespec My_diff(struct timespec start, struct timespec end) {
    struct timespec temp;

    if ((end.tv_nsec - start.tv_nsec) < 0) {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}


// Retrieve the dimension of the data matrix from the filename
// nL = number of rows
// nC = number of columns
void get_dimension(char** argv, int &nL, int &nC) {
    int dimension[2] = {0, 0}, i = 0;
    char *pch = strtok(argv[1], "x/_");
    
    while (pch != NULL) {

        if (i == 1) {
            dimension[0] = atoi(pch);
        }
        if (i == 2) {
            dimension[1] = atoi(pch);
        }
       
        pch = strtok(NULL, "x/_");
        
        i++;
    }
    
    cout<<dimension[0]<<endl<<dimension[1]<<endl;
    nL = dimension[0] + 1;
    nC = dimension[0] + dimension[1] + 1;
 
}

//Function 1(Pivot row selection): 
//Count the number of positive values 
//if all are < 0 then solution is unbounded
//else find the MinRatio
void func1(vector<vector<double>> & standard_simplex_tableau,int& colNumb,int& constraintNumb,Max_Updated& max,Min_Updated& min,int& count)
{
    for (int i = 0; i < constraintNumb; i++) {
            if (standard_simplex_tableau[i][max.index] > 0.0) {
                double simplex_pivot = standard_simplex_tableau[i][colNumb] / standard_simplex_tableau[i][max.index];
                if (min.val > simplex_pivot) {
                    min.val = simplex_pivot;
                    min.index = i;
                }
            } else
                count++;
        }
}

//Function 2(UPDATE PIVOT ROW):
//Convert pivot element to 1 and rest to 0
void func2(vector<vector<double>>& standard_simplex_tableau,int& colNumb,int& constraintNumb,Max_Updated& max,Min_Updated& min,double& simplex_pivot)
{
    for (int j = 0; j <= (colNumb); j++) {
            standard_simplex_tableau[min.index][j] = standard_simplex_tableau[min.index][j] / simplex_pivot;
        }
}

//Function 3(UPDATE ALL OTHER ROWS EXCEPT PIVOT ROW)
//Perform row operations to modify all other rows except the pivot row
void func3(vector<vector<double>> & standard_simplex_tableau,int& colNumb,int& constraintNumb,Max_Updated& max,Min_Updated& min)
{
    for (int i = 0; i < constraintNumb; i++) {
            if (i != min.index) {
                double simplex_pivot2 = -standard_simplex_tableau[i][max.index];

                for (int j = 0; j <= colNumb; j++) {
                    standard_simplex_tableau[i][j] = (simplex_pivot2 * standard_simplex_tableau[min.index][j]) + standard_simplex_tableau[i][j];
                }
            }
        }
}



//Function 4(Pivot column selection):
//Update the reduced costs 
//Find the maximum value for next iteration
void func4(vector<vector<double>> & standard_simplex_tableau,int& colNumb,int& constraintNumb,Max_Updated& max,Min_Updated& min,double& simplex_pivot3,int& count_neg)
{
    for (int j = 0; j <= colNumb; j++) {
            standard_simplex_tableau[constraintNumb][j] = (simplex_pivot3 * standard_simplex_tableau[min.index][j]) + standard_simplex_tableau[constraintNumb][j];
            if (j < colNumb && standard_simplex_tableau[constraintNumb][j] < 0.0) {
                count_neg++;
                if (max.val < (-standard_simplex_tableau[constraintNumb][j])) {
                    max.val = -standard_simplex_tableau[constraintNumb][j];
                    max.index = j;
                }
            }
        }
}

int main(int argc, char** argv) {
    double ONE_SECOND_IN_NANOSECONDS = 1000000000;
    int numbThreads, constraintNumb, colNumb, ni = 0, chunk = 1,nL,nC;
    int count = 0, count_neg = 0;
    ifstream file(argv[1]);
    get_dimension(argv, nL, nC);
    vector<vector<double>> standard_simplex_tableau(nL+1,vector<double> (nC+1,0));
    cout<<nL<<"  "<<nC<<endl; 
    int NMAX=(nL + nC);
    
    
    constraintNumb=nL;
    colNumb=nC;
    
    string line;
    int lin=0,i=0;
    
 
    while (getline(file, line)) {
        istringstream iss(line);
        double value;
        i=0;
        while (iss >> value && i<nC)
        {
          standard_simplex_tableau[lin][i]=value;
          i++;
        }
        lin++;
        
    }
    
    
    constraintNumb=nL;
    colNumb=nC;
    
    constraintNumb--;
    colNumb--;
    struct timespec timeTotalInit, timeTotalEnd,local1,local2,time1,time2,time3,time4;
    double  time_1=0,time_2=0,time_3=0,time_4=0;

    bool flag=true;
    Max_Updated max;
    Min_Updated min;

    if (clock_gettime(CLOCK_REALTIME, &timeTotalInit)) {
        perror("clock gettime");
        exit(EXIT_FAILURE);
    }


    double simplex_pivot, simplex_pivot2, simplex_pivot3;
    int j;

    //LOOP 1(Pivot column selection):
    //finds the maximum value from reduced cost row
    for (j = 0; j <= colNumb; j++)  
        if (standard_simplex_tableau[constraintNumb][j] < 0.0 && max.val < (-standard_simplex_tableau[constraintNumb][j])) {
            max.val = -standard_simplex_tableau[constraintNumb][j];
            max.index = j;
        }



    max.val = 0;

    do {

        if (clock_gettime(CLOCK_REALTIME, &local1)) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

            
        func1(standard_simplex_tableau,colNumb,constraintNumb,max,min,count);
        
        if (count == constraintNumb) {
            cout<<"Solution not found"<<endl;
            flag=false;
        } 
        else
            count = 0;
            
        count_neg = 0;
        simplex_pivot = standard_simplex_tableau[min.index][max.index];
        simplex_pivot3 = -standard_simplex_tableau[constraintNumb][max.index];


        if (clock_gettime(CLOCK_REALTIME, &local2)) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }
        time1 = My_diff(local1, local2);
        time_1+= (time1.tv_sec + (double) time1.tv_nsec / ONE_SECOND_IN_NANOSECONDS);


        if (clock_gettime(CLOCK_REALTIME, &local1)) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

        func2(standard_simplex_tableau,colNumb,constraintNumb,max,min,simplex_pivot);
        
        if (clock_gettime(CLOCK_REALTIME, &local2)) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }
        time2 = My_diff(local1, local2);
        time_2+= (time2.tv_sec + (double) time2.tv_nsec / ONE_SECOND_IN_NANOSECONDS);


        func3(standard_simplex_tableau,colNumb,constraintNumb,max,min);
        
        if (clock_gettime(CLOCK_REALTIME, &local1)) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

        time3 = My_diff(local2, local1);
        time_3+= (time3.tv_sec + (double) time3.tv_nsec / ONE_SECOND_IN_NANOSECONDS);


        func4(standard_simplex_tableau,colNumb,constraintNumb,max,min,simplex_pivot3,count_neg);
        if (clock_gettime(CLOCK_REALTIME, &local2)) {
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }
        time4 = My_diff(local1, local2);
        time_4+= (time4.tv_sec + (double) time4.tv_nsec / ONE_SECOND_IN_NANOSECONDS);

        ni++;
        max.val = 0.0;
        min.val = HUGE_VAL;
        
    } while (count_neg && ni<NMAX  && flag);
    

    if (clock_gettime(CLOCK_REALTIME, &timeTotalEnd)) {
        perror("clock gettime");
        exit(EXIT_FAILURE);
    }

    
    struct timespec time = My_diff(timeTotalInit, timeTotalEnd);
    double processTime = (time.tv_sec + (double) time.tv_nsec / ONE_SECOND_IN_NANOSECONDS);

    printf("%f %f ", processTime / ni, processTime);
    printf("%d %f \n", ni, standard_simplex_tableau[constraintNumb][colNumb]);
    cout<<"time 1 : "<<time_1<<endl;
    cout<<"time 2 : "<<time_2<<endl;
    cout<<"time 3 : "<<time_3<<endl;
    cout<<"time 4 : "<<time_4<<endl;
    
    return 0;

}