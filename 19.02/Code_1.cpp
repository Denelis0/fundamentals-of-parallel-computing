#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <windows.h>

const int SIZE = 100; // Размер матриц
using MatrixType = std::vector<std::vector<int>>;

// Функция для инициализации матрицы случайными числами
void fillMatrixWithRandomValues(MatrixType& matrix) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            matrix[row][col] = rand() % 100;
        }
    }
}

// Однопоточная функция для умножения матриц
void multiplyWithoutParallelism(const MatrixType& mat1, const MatrixType& mat2, MatrixType& result) {
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            for (int k = 0; k < SIZE; ++k) {
                result[row][col] += mat1[row][k] * mat2[k][col];
            }
        }
    }
}

// Умножение матриц с OpenMP с использованием статического расписания
void multiplyWithOmpStatic(const MatrixType& mat1, const MatrixType& mat2, MatrixType& result) {
#pragma omp parallel for schedule(static)
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            for (int k = 0; k < SIZE; ++k) {
                result[row][col] += mat1[row][k] * mat2[k][col];
            }
        }
    }
}

// Умножение матриц с OpenMP с использованием динамического расписания
void multiplyWithOmpDynamic(const MatrixType& mat1, const MatrixType& mat2, MatrixType& result) {
#pragma omp parallel for schedule(dynamic, 4)
    for (int row = 0; row < SIZE; ++row) {
        for (int col = 0; col < SIZE; ++col) {
            for (int k = 0; k < SIZE; ++k) {
                result[row][col] += mat1[row][k] * mat2[k][col];
            }
        }
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    SetConsoleOutputCP(65001);

    MatrixType matrixA(SIZE, std::vector<int>(SIZE)), matrixB(SIZE, std::vector<int>(SIZE)), resultMatrix(SIZE, std::vector<int>(SIZE, 0));

    fillMatrixWithRandomValues(matrixA);
    fillMatrixWithRandomValues(matrixB);

    // Однопоточное умножение
    auto startTime = std::chrono::high_resolution_clock::now();
    multiplyWithoutParallelism(matrixA, matrixB, resultMatrix);
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "Однопоточное выполнение: " << std::chrono::duration<double>(endTime - startTime).count() << " секунд" << std::endl;

    // Умножение с использованием статики
    resultMatrix.assign(SIZE, std::vector<int>(SIZE, 0));
    startTime = std::chrono::high_resolution_clock::now();
    multiplyWithOmpStatic(matrixA, matrixB, resultMatrix);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "OpenMP (статическое расписание): " << std::chrono::duration<double>(endTime - startTime).count() << " секунд" << std::endl;

    // Умножение с использованием OpenMP динамики
    resultMatrix.assign(SIZE, std::vector<int>(SIZE, 0));
    startTime = std::chrono::high_resolution_clock::now();
    multiplyWithOmpDynamic(matrixA, matrixB, resultMatrix);
    endTime = std::chrono::high_resolution_clock::now();
    std::cout << "OpenMP (динамическое расписание): " << std::chrono::duration<double>(endTime - startTime).count() << " секунд" << std::endl;

    return 0;
}
