#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace chrono;

vector<int> matrixVectorMultiplicationSequential(const vector<vector<int>>& matrix, const vector<int>& vec) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    vector<int> result(rows, 0);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i] += matrix[i][j] * vec[j];
        }
    }

    return result;
}

vector<int> matrixVectorMultiplicationParallel(const vector<vector<int>>& matrix, const vector<int>& vec) {
    int rows = matrix.size();
    int cols = matrix[0].size();
    vector<int> result(rows, 0);

#pragma omp parallel for
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i] += matrix[i][j] * vec[j];
        }
    }

    return result;
}

int main() {
    setlocale(LC_ALL, "RU");

    const int rowCount = 10000;
    const int colCount = 10000;

    // Инициализация матрицы и вектора
    vector<vector<int>> matrix(rowCount, vector<int>(colCount, 1000));
    vector<int> vectorData(colCount, 2000);

    // Параллельное выполнение
#pragma omp parallel
    {
    #pragma omp single
        cout << "Количество используемых потоков: " << omp_get_num_threads() << endl;
    }

    // Время выполнения последовательного умножения
    auto startTime = high_resolution_clock::now();
    vector<int> sequentialResult = matrixVectorMultiplicationSequential(matrix, vectorData);
    auto endTime = high_resolution_clock::now();
    auto sequentialDuration = duration_cast<milliseconds>(endTime - startTime);
    cout << "Время выполнения последовательного умножения: " << sequentialDuration.count() << " мс" << endl;

    // Время выполнения параллельного умножения
    startTime = high_resolution_clock::now();
    vector<int> parallelResult = matrixVectorMultiplicationParallel(matrix, vectorData);
    endTime = high_resolution_clock::now();
    auto parallelDuration = duration_cast<milliseconds>(endTime - startTime);
    cout << "Время выполнения параллельного умножения: " << parallelDuration.count() << " мс" << endl;

    return 0;
}
