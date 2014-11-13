#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

gpu::GpuMat gpuMorphOps(gpu::GpuMat &thresh){

	gpu::GpuMat tempMat1, tempMat2;
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_ELLIPSE,Size(10,10));
	
	gpu::erode(thresh,tempMat1,erodeElement);
	gpu::erode(tempMat1,tempMat2,erodeElement);
	//you have to comment these back in for it to work
	gpu::dilate(tempMat2,tempMat1,dilateElement);
	gpu::dilate(tempMat1,tempMat2,dilateElement);

	gpu::threshold(tempMat2, tempMat1, 20, 255, THRESH_BINARY);

	return tempMat1;
}
void displayGPUImage(char* name, gpu::GpuMat mat){
	Mat temp;
	mat.download(temp);
	imshow(name, temp);
}
vector<Vec3f> removeConcentricCircles(vector<Vec3f> circles){
	
	int center[circles.size()][2];
	for(int i = 0; i < circles.size(); i++){
		int x = int(circles[i][0]);
		int y = int(circles[i][1]);
		//Do stuff here
	}
}
void GPUprocess(VideoCapture cap){
	Mat frame, temp, out;
	gpu::GpuMat gpuFrame, gpuOut, gpuTemp, gpuIn;

	bool bSuccess = cap.read(frame);
	if(!bSuccess){
		cout << "Cannot read the frame" << endl;
		return;
	}

	gpuIn.upload(frame);
	
	gpu::pyrDown(gpuIn, gpuFrame);
	pyrDown(frame, frame);
		
	vector <gpu::GpuMat> channels;
	gpu::split(gpuFrame, channels);
	gpu::add(channels[0],channels[1],gpuTemp);
	gpu::subtract(channels[2],gpuTemp,gpuFrame);
	
	gpuFrame = gpuMorphOps(gpuFrame);
	//displayGPUImage("GPU out after circles", gpuFrame);

	gpu::GpuMat gpuCircles;
	vector<Vec3f> cpuCircles;
	gpu::HoughCircles(gpuFrame, gpuCircles, CV_HOUGH_GRADIENT, 1, 50, 20, 13, 1, frame.rows/10);
	gpu::HoughCirclesDownload(gpuCircles,cpuCircles);
	
	cpuCircles = removeConcentricCircles(cpuCircles);

	
	for( size_t i = 0; i < cpuCircles.size(); i++ )
	{
		Point center(cvRound(cpuCircles[i][0]), cvRound(cpuCircles[i][1]));
		int radius = cvRound(cpuCircles[i][2]);
		// draw the circle center
		circle( frame, center, 3, Scalar(0,128,128), -1, 8, 0 );
		// draw the circle outline
		circle( frame, center, radius, Scalar(0,128,128), 3, 8, 0 );
	}
	imshow("GPU in", frame);
}

int main(int argc, char* argv[])
{
    VideoCapture cap("juggle.mov"); // open the video file for reading

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the video file" << endl;
         return -1;
    }
	
    while(1)
    {
	GPUprocess(cap);		

	if(waitKey(30) == 27)
	{
                cout << "esc key is pressed by user" << endl; 
                break; 
       }
    }

    return 0;

}
