#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

const int N = 500;  // Размер матриц

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int proc_rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int rows_per_proc = N / num_procs;
    vector<int> matA(N * N), matB(N * N), resultMat(N * N, 0);

    // Инициализация матриц на процессе с рангом 0
    if (proc_rank == 0) {
        srand(time(nullptr));
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                matA[i * N + j] = rand() % 10;
                matB[i * N + j] = rand() % 10;
            }
        }
    }

    // Разделение матрицы A между процессами
    vector<int> localMatA(rows_per_proc * N);
    MPI_Scatter(matA.data(), rows_per_proc * N, MPI_INT, localMatA.data(), rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Рассылка матрицы B всем процессам
    MPI_Bcast(matB.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> localRes(rows_per_proc * N, 0);

    // Умножение матриц
    for (int i = 0; i < rows_per_proc; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                localRes[i * N + j] += localMatA[i * N + k] * matB[k * N + j];
            }
        }
    }

    // Сбор результата на процессе 0
    MPI_Gather(localRes.data(), rows_per_proc * N, MPI_INT, resultMat.data(), rows_per_proc * N, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();
    if (proc_rank == 0) {
        cout << "First 5x5 elements of the result matrix:" << endl;
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                cout << resultMat[i * N + j] << " ";
            }
            cout << endl;
        }
        double end_time = MPI_Wtime();
        cout << "Total time taken: " << (end_time - start_time) << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}
