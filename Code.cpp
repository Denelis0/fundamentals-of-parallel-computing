#include <opencv2/opencv.hpp>
#include <iostream>
#include <omp.h>
#include <cmath>

using namespace cv;
using namespace std;

void drawFractalCarpet(Mat& canvas, int startX, int startY, int size, int currentLevel, int maxLevel) noexcept {
    if (currentLevel >= maxLevel) return;

    int subSquareSize = size / 3;

    rectangle(canvas,
        Point(startX + subSquareSize, startY + subSquareSize),
        Point(startX + 2 * subSquareSize, startY + 2 * subSquareSize),
        Scalar(0, 0, 0),
        FILLED);

#pragma omp parallel for collapse(2)
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (row == 1 && col == 1) continue; 
            if (subSquareSize >= 3) { 
                drawFractalCarpet(canvas,
                    startX + row * subSquareSize,
                    startY + col * subSquareSize,
                    subSquareSize,
                    currentLevel + 1,
                    maxLevel);
            }
        }
    }
}

bool checkIfPowerOfThree(int number) {
    if (number < 1) return false;
    double logBase3 = log(number) / log(3);
    return (pow(3, floor(logBase3)) == number);
}

int main() {
    try {
        const int IMAGE_SIZE = 729;     
        const int MAX_LEVEL = 5;
        const int THREAD_COUNT = 8;

        if (!checkIfPowerOfThree(IMAGE_SIZE)) {
            throw runtime_error("Размер должен быть степенью тройки");
        }
        if (MAX_LEVEL < 1 || THREAD_COUNT < 1) {
            throw invalid_argument("Некорректные параметры глубины или потоков");
        }

        omp_set_num_threads(THREAD_COUNT);

        Mat canvas(IMAGE_SIZE, IMAGE_SIZE, CV_8UC3, Scalar(255, 0, 0));  

        double startTime = omp_get_wtime();
        drawFractalCarpet(canvas, 0, 0, IMAGE_SIZE, 0, MAX_LEVEL);
        cout << "Время выполнения: " << omp_get_wtime() - startTime << " сек.\n";

        imshow("Sierpinski_Carpet", canvas);
        waitKey(0);

    }
    catch (const exception& e) {
        cerr << "Ошибка: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
