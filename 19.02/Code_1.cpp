#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace chrono;

const int SIZE = 100;
const int BLOCK_SIZE = 32; // Размер блока для блочного умножения
const int NUM_THREADS = 8;

// Функция для создания матрицы, заполненной единицами
vector<vector<int>> create_matrix(int size) {
    return vector<vector<int>>(size, vector<int>(size, 1));
}

// Блочное умножение матриц (однопоточное)
vector<vector<int>> multiply_single_threaded(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(SIZE, vector<int>(SIZE, 0));

    for (int i = 0; i < SIZE; i += BLOCK_SIZE) {
        for (int j = 0; j < SIZE; j += BLOCK_SIZE) {
            for (int ii = i; ii < min(i + BLOCK_SIZE, SIZE); ++ii) {
                for (int jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj) {
                    int sum = 0;
                    for (int k = 0; k < SIZE; ++k) {
                        sum += A[ii][k] * B[k][jj];
                    }
                    C[ii][jj] = sum;
                }
            }
        }
    }

    return C;
}

// Блочное умножение с параллельным выполнением
vector<vector<int>> multiply_parallel(const vector<vector<int>>& A, const vector<vector<int>>& B) {
    vector<vector<int>> C(SIZE, vector<int>(SIZE, 0));

    // Установка количества потоков OpenMP
    omp_set_num_threads(NUM_THREADS);

    // Параллельная обработка блоков
#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < SIZE; i += BLOCK_SIZE) {
        for (int j = 0; j < SIZE; j += BLOCK_SIZE) {
            for (int ii = i; ii < min(i + BLOCK_SIZE, SIZE); ++ii) {
                for (int jj = j; jj < min(j + BLOCK_SIZE, SIZE); ++jj) {
                    int sum = 0;
                    for (int k = 0; k < SIZE; ++k) {
                        sum += A[ii][k] * B[k][jj];
                    }
                    C[ii][jj] = sum;
                }
            }
        }
    }

    return C;
}

int main() {
    // Создание матриц
    vector<vector<int>> A = create_matrix(SIZE);
    vector<vector<int>> B = create_matrix(SIZE);

    // Однопоточное умножение
    auto start = high_resolution_clock::now();
    auto C1 = multiply_single_threaded(A, B);
    auto end = high_resolution_clock::now();
    cout << "Single-threaded time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

    // Параллельное умножение
    start = high_resolution_clock::now();
    auto C2 = multiply_parallel(A, B);
    end = high_resolution_clock::now();
    cout << "Parallel time: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

    return 0;
}
