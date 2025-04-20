#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <vector>

void detectSmiles(cv::Mat& grayFaceRegion, cv::CascadeClassifier& smileClassifier, std::vector<cv::Rect>& detectedSmiles) {
	smileClassifier.detectMultiScale(grayFaceRegion, detectedSmiles, 1.24, 15, 0, cv::Size(40, 40));
}

void detectEyes(cv::Mat& grayFaceRegion, cv::CascadeClassifier& eyeClassifier, std::vector<cv::Rect>& detectedEyes) {
	eyeClassifier.detectMultiScale(grayFaceRegion, detectedEyes, 1.1, 10, 0, cv::Size(50, 50));
}

int main() {
	cv::CascadeClassifier faceClassifier, eyeClassifier, smileClassifier;

	if (
		!faceClassifier.load("./haarcascade_frontalface_default.xml") ||
		!eyeClassifier.load("./haarcascade_eye.xml") ||
		!smileClassifier.load("./haarcascade_smile.xml")
		) {
		std::cerr << "fail to load haar cascade!" << std::endl;
		return -1;
	}

	cv::VideoCapture video("D:\\CV\\Tue1\\video.mp4");

	if (!video.isOpened()) {
		std::cerr << "fail to load video!" << std::endl;
		return -1;
	}

	cv::Mat currentFrame;
	while (video.read(currentFrame)) {
		if (currentFrame.empty()) break;

		cv::Mat grayFrame;
		cv::cvtColor(currentFrame, grayFrame, cv::COLOR_BGR2GRAY);
		cv::equalizeHist(grayFrame, grayFrame);
		cv::GaussianBlur(grayFrame, grayFrame, cv::Size(5, 5), 0);

		std::vector<cv::Rect> detectedFaces;
		faceClassifier.detectMultiScale(grayFrame, detectedFaces, 1.1, 5, 0, cv::Size(150, 150));

		for (const auto& faceRect : detectedFaces) {
			cv::rectangle(currentFrame, faceRect, cv::Scalar(255, 0, 0), 2);

			cv::Mat grayFaceRegion = grayFrame(faceRect);
			cv::Mat colorFaceRegion = currentFrame(faceRect);

			std::vector<cv::Rect> detectedEyes, detectedSmiles;

			std::thread eyeThread(detectEyes, std::ref(grayFaceRegion), std::ref(eyeClassifier), std::ref(detectedEyes));
			std::thread smileThread(detectSmiles, std::ref(grayFaceRegion), std::ref(smileClassifier), std::ref(detectedSmiles));

			eyeThread.join();
			smileThread.join();

			for (const auto& eye : detectedEyes) {
				cv::rectangle(colorFaceRegion, eye, cv::Scalar(0, 0, 255), 2);
			}

			for (const auto& smile : detectedSmiles) {
				cv::rectangle(colorFaceRegion, smile, cv::Scalar(0, 255, 0), 2);
			}
		}

		cv::imshow("face detect", currentFrame);
		if (cv::waitKey(30) >= 0) break;
	}

	video.release();
	cv::destroyAllWindows();
	return 0;
}
