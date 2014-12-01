/*
* File:   main.cpp
* Author: sagar
*
* Created on 10 September, 2012, 7:48 PM
*/
 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <iostream>
using namespace cv;
using namespace std;
 
int main() {
	VideoCapture stream1(0);   //0 is the id of video device.0 if you have only one camera.
	//stream1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//stream1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	if (!stream1.isOpened()) { //check if video device has been initialised
		cout << "cannot open camera";
	}
	//unconditional loop
	while (true) {
		Mat cameraFrame;
		stream1.read(cameraFrame);

		

		gpu::GpuMat d_cameraFrame(cameraFrame);
		gpu::GpuMat d_dst;
		gpu::cvtColor(d_cameraFrame, d_dst, CV_BGR2GRAY );

		gpu::GpuMat d_circles;
		gpu::HoughCircles(d_dst, d_circles, CV_HOUGH_GRADIENT, 1, 60, 200, 100, 2, 2048, 900);
		
		vector<Vec3f> circles;
		gpu::HoughCirclesDownload(d_circles, circles);

		//gpu::GpuMat d_cameraFrame(cameraFrame);
		//gpu::GpuMat d_dst;
		//gpu::bilateralFilter(d_cameraFrame, d_dst, -1, 50, 7);
		//gpu::Canny(d_dst, d_dst, 35, 200, 3);

		
		Mat dst(d_dst);
		imshow("cam", dst);

		/// Draw the circles detected
		for( size_t i = 0; i < circles.size(); i++ ){
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( dst, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( dst, center, radius, Scalar(0,0,255), 3, 8, 0 );
		}

		/// Show your results
		//Mat dst(d_dst);
		//imshow("cam", dst);
		//namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
		imshow( "Hough Circle Transform Demo", dst );

		//Mat dst(d_dst);
		//imshow("cam", dst);
		if (waitKey(30) >= 0)
			break;
		}
	return 0;
}
