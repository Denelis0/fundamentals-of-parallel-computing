#include <opencv2/opencv.hpp>
#include <iostream>
#include <omp.h>
#include <cmath>

using namespace cv;
using namespace std;

bool isPowerOfThree(int num) {
    if (num < 1) return false;
    double logResult = log(num) / log(3);
    return (pow(3, floor(logResult)) == num);
}

void drawPattern(Mat& image, int posX, int posY, int dim, int level, int maxLevel) noexcept {
    if (level >= maxLevel) return;

    int blockSize = dim / 3;

    rectangle(image,
        Point(posX + blockSize, posY + blockSize),
        Point(posX + 2 * blockSize, posY + 2 * blockSize),
        Scalar(0, 0, 0), // Black color for center squares
        FILLED);

#pragma omp parallel for collapse(2)
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (i == 1 && j == 1) continue; // Skip the middle square
            if (blockSize >= 3) { // Check for minimum size
                drawPattern(image,
                    posX + i * blockSize,
                    posY + j * blockSize,
                    blockSize,
                    level + 1,
                    maxLevel);
            }
        }
    }
}

int main() {
    try {
        const int IMAGE_DIM = 729;    // 3^6
        const int MAX_DEPTH = 5;
        const int THREAD_COUNT = 8;

        if (!isPowerOfThree(IMAGE_DIM)) {
            throw runtime_error("Размер должен быть степенью тройки");
        }
        if (MAX_DEPTH < 1 || THREAD_COUNT < 1) {
            throw invalid_argument("Некорректные параметры глубины или потоков");
        }

        omp_set_num_threads(THREAD_COUNT);

        // Creating blue background
        Mat frame(IMAGE_DIM, IMAGE_DIM, CV_8UC3, Scalar(255, 0, 0));  // Blue background

        // Start the drawing process
        double startTime = omp_get_wtime();
        drawPattern(frame, 0, 0, IMAGE_DIM, 0, MAX_DEPTH);
        cout << "Execution Time: " << omp_get_wtime() - startTime << " seconds.\n";

        imshow("Fractal Pattern", frame);
        waitKey(0);

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
