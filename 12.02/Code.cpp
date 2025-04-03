#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

int counter_without_sync = 0;
int counter_with_sync = 0;

// Мьютекс для синхронизации доступа к counter_with_sync
std::mutex sync_mutex;

// Функция без синхронизации
void increase_without_sync(int thread_id) {
    for (int i = 0; i < 100000; ++i) {
        ++counter_without_sync;
    }
}

// Функция с синхронизацией
void increase_with_sync(int thread_id) {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(sync_mutex);
        ++counter_with_sync;
    }
}

int main() {
    setlocale(LC_ALL, "RU");
    const int total_threads = 10;

    std::vector<std::thread> threads_no_sync;
    std::vector<std::thread> threads_with_sync;

    // Замер времени для без синхронизации
    auto start_no_sync = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < total_threads; ++i) {
        threads_no_sync.push_back(std::thread(increase_without_sync, i));
    }

    for (auto& t : threads_no_sync) {
        t.join();
    }

    auto end_no_sync = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_no_sync = end_no_sync - start_no_sync;

    // Замер времени для с синхронизацией
    auto start_with_sync = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < total_threads; ++i) {
        threads_with_sync.push_back(std::thread(increase_with_sync, i));
    }

    for (auto& t : threads_with_sync) {
        t.join();
    }

    auto end_with_sync = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration_with_sync = end_with_sync - start_with_sync;

    std::cout << "\nРезультаты:\n";
    std::cout << "Конечное значение counter_without_sync: " << counter_without_sync << std::endl;
    std::cout << "Время выполнения без синхронизации: " << duration_no_sync.count() << " секунд\n";

    std::cout << "Конечное значение counter_with_sync: " << counter_with_sync << std::endl;
    std::cout << "Время выполнения с синхронизацией: " << duration_with_sync.count() << " секунд\n";


    return 0;
}
