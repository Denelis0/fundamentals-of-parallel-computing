#include <iostream>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <windows.h>

using namespace std;

int numDivisions = 1000000; // Кол-во разбиений

// Последовательное вычисление
double computeSequentially() {
    double startPoint = 0.0, endPoint = 3.1415 / 2;
    double stepSize = (endPoint - startPoint) / numDivisions;
    double totalSum = 0.0;

    for (int currentIndex = 0; currentIndex < numDivisions; currentIndex++) {
        double currentX = startPoint + (currentIndex + 0.5) * stepSize;
        totalSum += tan(currentX);
    }
    return totalSum * stepSize;
}

// Параллельное вычисление
double computeInParallel() {
    double startPoint = 0.0, endPoint = 3.1415 / 2;
    double stepSize = (endPoint - startPoint) / numDivisions;
    double totalSum = 0.0;

#pragma omp parallel for reduction(+:totalSum)
    for (int currentIndex = 0; currentIndex < numDivisions; currentIndex++) {
        double currentX = startPoint + (currentIndex + 0.5) * stepSize;
        totalSum += tan(currentX);
    }
    return totalSum * stepSize;
}

// Измерение времени выполнения функции
double calculateExecutionTime(double (*func)()) {
    auto startTime = chrono::high_resolution_clock::now();
    func();
    auto endTime = chrono::high_resolution_clock::now();
    return chrono::duration<double>(endTime - startTime).count();
}

int main() {
    setlocale(LC_ALL, "RU");
    SetConsoleOutputCP(65001);

    omp_set_num_threads(4);

    // Параллельное вычисление
    cout << "Параллельное вычисление: " << calculateExecutionTime(computeInParallel) << " секунд" << endl;

    // Последовательное вычисление
    cout << "Последовательное вычисление: " << calculateExecutionTime(computeSequentially) << " секунд" << endl;

    return 0;
}
