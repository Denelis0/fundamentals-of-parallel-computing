#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <complex>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

float calculateMandelbrot(double coordX, double coordY) {
	std::complex<double> z(0.0, 0.0);
	std::complex<double> c(coordX, coordY);

	int iterationCount = 0;

	while (std::abs(z) < 2.0 && iterationCount < 1000) {
		z = z * z + c;
		iterationCount++;
	}

	return iterationCount;
}

cv::Vec3b mapColor(float value) {
	value = value / 1000;
	unsigned char blueComponent = static_cast<unsigned char>(255.0 * std::pow(value, 0.3));
	return cv::Vec3b(blueComponent, 0, 0);  // Blue tone
}

void generateFractal(cv::Mat& outputImage, int offsetY, int sectionHeight) {
	for (int y = 0; y < sectionHeight; ++y) {
		for (int x = 0; x < 800; ++x) {
			double realPart = -2.5 + x * (1.0 + 2.5) / 800;
			double imagPart = -1.5 + (offsetY + y) * (1.5 + 1.5) / 800;

			float iter = calculateMandelbrot(realPart, imagPart);

			cv::Vec3b pixelColor;
			if (iter >= 1000)
				pixelColor = cv::Vec3b(0, 0, 0);
			else
				pixelColor = mapColor(iter);

			outputImage.at<cv::Vec3b>(y, x) = pixelColor;
		}
	}
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int processRank, processCount;

	MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
	MPI_Comm_size(MPI_COMM_WORLD, &processCount);

	if (processRank == 0) {
		auto timeStart = std::chrono::high_resolution_clock::now();

		cv::Mat serialImage(800, 800, CV_8UC3);
		generateFractal(serialImage, 0, 800);

		auto timeEnd = std::chrono::high_resolution_clock::now();
		double elapsed = std::chrono::duration<double>(timeEnd - timeStart).count();
		std::cout << "Serial time: " << elapsed << " sec" << std::endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);
	int baseRows = 800 / processCount;
	int extraRows = 800 % processCount;

	int currentOffset = baseRows * processRank;
	if (processRank < extraRows) currentOffset += processRank;
	else currentOffset += extraRows;

	int localHeight = baseRows;
	if (processRank < extraRows) localHeight += 1;

	cv::Mat localImage(localHeight, 800, CV_8UC3);

	auto mpiStart = std::chrono::high_resolution_clock::now();

	generateFractal(localImage, currentOffset, localHeight);

	if (processRank == 0) {
		cv::Mat resultImage(800, 800, CV_8UC3);
		localImage.copyTo(resultImage(cv::Rect(0, currentOffset, 800, localHeight)));

		for (int i = 1; i < processCount; ++i) {
			int otherOffset = baseRows * i;
			if (i < extraRows) otherOffset += i;
			else otherOffset += extraRows;

			int otherRows = baseRows;
			if (i < extraRows) otherRows += 1;

			int dataSize = otherRows * 800 * 3;
			std::vector<unsigned char> buffer(dataSize);

			MPI_Recv(buffer.data(), dataSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			cv::Mat temp(otherRows, 800, CV_8UC3, buffer.data());
			temp.copyTo(resultImage(cv::Rect(0, otherOffset, 800, otherRows)));
		}

		auto mpiEnd = std::chrono::high_resolution_clock::now();
		double elapsed = std::chrono::duration<double>(mpiEnd - mpiStart).count();
		std::cout << "MPI time: " << elapsed << " sec" << std::endl;

		cv::imwrite("mandelbrot_mpi_blue.png", resultImage);
		cv::imshow("Mandelbrot MPI Blue", resultImage);
		cv::waitKey(0);
	} else {
		int sendSize = localHeight * 800 * 3;
		MPI_Send(localImage.data, sendSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
