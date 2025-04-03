#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

const int ARRAY_SIZE = 1000; // Размер массива

// Заполнение массива
void generateRandomArray(vector<int>& data) {
    for (auto& element : data) {
        element = rand() % 1000;
    }
}

// Последовательная сортировка
void sequentialOddEvenSort(vector<int>& data) {
    bool sorted = false;
    int n = data.size();
    while (!sorted) {
        sorted = true;

        for (int i = 1; i < n - 1; i += 2) {
            if (data[i] > data[i + 1]) {
                swap(data[i], data[i + 1]);
                sorted = false;
            }
        }

        for (int i = 0; i < n - 1; i += 2) {
            if (data[i] > data[i + 1]) {
                swap(data[i], data[i + 1]);
                sorted = false;
            }
        }
    }
}

// Параллельная сортировка
void parallelOddEvenSort(vector<int>& data) {
    bool sorted = false;
    int n = data.size();
    while (!sorted) {
        sorted = true;

#pragma omp parallel for shared(data, sorted)
        for (int i = 1; i < n - 1; i += 2) {
            if (data[i] > data[i + 1]) {
                swap(data[i], data[i + 1]);
#pragma omp critical
                sorted = false;
            }
        }

#pragma omp parallel for shared(data, sorted)
        for (int i = 0; i < n - 1; i += 2) {
            if (data[i] > data[i + 1]) {
                swap(data[i], data[i + 1]);
#pragma omp critical
                sorted = false;
            }
        }
    }
}

// Измерение времени выполнения функции
double measureExecutionTime(void (*func)(vector<int>&), vector<int>& data) {
    auto start = chrono::high_resolution_clock::now();
    func(data);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double>(end - start).count();
}

int main() {

    setlocale(LC_ALL, "RU");

    SetConsoleOutputCP(65001);
    omp_set_num_threads(4);
    vector<int> sequentialData(ARRAY_SIZE);
    vector<int> parallelData(ARRAY_SIZE);
    generateRandomArray(sequentialData);
    parallelData = sequentialData;
    cout << "Время последовательной сортировки: " << measureExecutionTime(sequentialOddEvenSort, sequentialData) << " секунд" << endl;
    cout << "Время параллельной сортировки: " << measureExecutionTime(parallelOddEvenSort, parallelData) << " секунд" << endl;

    return 0;
}
