#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#define DEFAULT_THREAD_COUNT 4
typedef long long ll;
ll calculateFactorialSequential(int num)
{
    if (num < 0)
    {
        throw std::invalid_argument("Ошибка: число меньше 0");
    }
    if (num <= 1)
    {
        return 1;
    }
    ll result = 1;
    for (int i = 2; i <= num; i++)
    {
        result *= i;
    }
    return result;
}
std::mutex calculation_mutex;
void calculatePartialFactorial(std::vector<ll>& partialResults, ll start, int segmentLength, int limit)
{
    if (start > limit)
    {
        return;
    }
    ll partialResult = start;
    for (int i = 1; i < segmentLength; i++)
    {
        if (start + i > limit)
        {
            break;
        }
        partialResult *= (start + i);
    }
    std::lock_guard<std::mutex> lock(calculation_mutex);
    partialResults.push_back(partialResult);
}
ll calculateFactorialParallel(int num, int threads)
{
    if (num < 0)
    {
        throw std::invalid_argument("Ошибка: число меньше 0");
    }
    if (num <= 1)
    {
        return 1;
    }
    ll result = 1;
    int segmentSize = std::ceil(1.0 * num / threads);
    std::vector<std::thread> threadPool;
    std::vector<ll> partialResults;
    for (int i = 0; i < threads; i++)
    {
        threadPool.emplace_back(std::thread(calculatePartialFactorial, std::ref(partialResults), segmentSize * i + 1, segmentSize, num));
    }
    for (auto& t : threadPool)
    {
        t.join();
    }
    for (ll partial : partialResults)
    {
        result *= partial;
    }
    return result;
}
int main(int argc, const char** argv)
{
    std::system("chcp 65001 > nul");
    setlocale(LC_ALL, "RU");
    int threadCount = DEFAULT_THREAD_COUNT;
    if (argc > 1)
    {
        threadCount = std::atoi(argv[1]);
        if (threadCount <= 0)
        {
            std::cerr << "Ошибка: Некорректное количество потоков" << std::endl;
            return 1;
        }
    }
    int number;
    std::cout << "Введите число для вычисления факториала: ";
    if (!(std::cin >> number))
    {
        std::cerr << "Ошибка: Введено некорректное значение" << std::endl;
        return 1;
    }
    try
    {
        ll sequentialResult = calculateFactorialSequential(number);
        ll parallelResult = calculateFactorialParallel(number, threadCount);
        assert(sequentialResult == parallelResult);

        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "Последовательный факториал: " << calculateFactorialSequential(number) << std::endl;
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Время выполнения: " << duration.count() << "мкс" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        std::cout << "Параллельный факториал: " << calculateFactorialParallel(number, threadCount) << std::endl;
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Время выполнения: " << duration.count() << "мкс" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
