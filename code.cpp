#include <iostream>
#include <thread>
#include <chrono>

// Функция для вычисления факториала с одним потоком
unsigned long long compute_factorial_single_thread(unsigned long long num) {
    unsigned long long result = 1;
    for (unsigned long long i = 1; i <= num; ++i) {
        result *= i;
    }
    return result;
}

// Частичный факториал для многопоточной обработки
void compute_partial_factorial(unsigned long long start, unsigned long long end, unsigned long long& partial_result) {
    unsigned long long temp = 1;
    for (unsigned long long i = start; i <= end; ++i) {
        temp *= i;
    }
    partial_result = temp;
}

// Функция для вычисления факториала с двумя потоками
unsigned long long compute_factorial_two_threads(unsigned long long num) {
    unsigned long long result_part1 = 1, result_part2 = 1;
    unsigned long long midpoint = num / 2;

    std::thread thread1(compute_partial_factorial, 1, midpoint, std::ref(result_part1));
    std::thread thread2(compute_partial_factorial, midpoint + 1, num, std::ref(result_part2));

    thread1.join();
    thread2.join();

    return result_part1 * result_part2;
}

int main() {
    setlocale(LC_ALL, "RU");
    unsigned long long num = 15;

    // Однопоток
    auto start_single_thread = std::chrono::high_resolution_clock::now();
    unsigned long long result_single_thread = compute_factorial_single_thread(num);
    auto end_single_thread = std::chrono::high_resolution_clock::now();
    auto duration_single_thread = std::chrono::duration_cast<std::chrono::nanoseconds>(end_single_thread - start_single_thread);

    // Двухпоток
    auto start_two_threads = std::chrono::high_resolution_clock::now();
    unsigned long long result_two_threads = compute_factorial_two_threads(num);
    auto end_two_threads = std::chrono::high_resolution_clock::now();
    auto duration_two_threads = std::chrono::duration_cast<std::chrono::nanoseconds>(end_two_threads - start_two_threads);

    // Вывод результатов
    std::cout << "Факториал " << num << " (1 поток) = " << result_single_thread << ", время: " << duration_single_thread.count() << " нс" << std::endl;
    std::cout << "Факториал " << num << " (2 потока) = " << result_two_threads << ", время: " << duration_two_threads.count() << " нс" << std::endl;

    return 0;
}
