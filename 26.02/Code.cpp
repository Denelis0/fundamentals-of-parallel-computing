#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>
#include <cstdlib> 

using namespace std;

// Параллельное суммирование элементов массива
long long calculateParallelSum(const vector<int>& data) {
    long long total = 0;

#pragma omp parallel for reduction(+:total)
    for (int index = 0; index < data.size(); ++index) {
        total += data[index];
    }

    return total;
}

// Однопоточное суммирование элементов массива
long long calculateSequentialSum(const vector<int>& data) {
    long long total = 0;

    for (int index = 0; index < data.size(); ++index) {
        total += data[index];
    }

    return total;
}

int main() {
    setlocale(LC_ALL, "RU");
    const size_t dataSize = 10000000;
    vector<int> data(dataSize);

    srand(static_cast<unsigned>(time(0)));
    for (size_t index = 0; index < dataSize; ++index) {
        data[index] = rand() % 100;
    }
    omp_set_num_threads(4);

    auto parallelStartTime = chrono::high_resolution_clock::now();
    long long parallelResult = calculateParallelSum(data);
    auto parallelEndTime = chrono::high_resolution_clock::now();
    chrono::duration<double> parallelDuration = parallelEndTime - parallelStartTime;

    auto sequentialStartTime = chrono::high_resolution_clock::now();
    long long sequentialResult = calculateSequentialSum(data);
    auto sequentialEndTime = chrono::high_resolution_clock::now();
    chrono::duration<double> sequentialDuration = sequentialEndTime - sequentialStartTime;

    cout << "Параллельная сумма: " << parallelResult << endl;
    cout << "Однопоточная сумма: " << sequentialResult << endl;
    cout << "Время параллельного суммирования: " << parallelDuration.count() << " секунд" << endl;
    cout << "Время однопоточного суммирования: " << sequentialDuration.count() << " секунд" << endl;
    return 0;
}
