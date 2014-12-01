#include "opencv2/gpu/gpu.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace cv;
using namespace std;

int erodeSize = 1;
int dilateSize = 1;
int addWeightIntBlue = 10;
int addWeightIntRed = 10;
double addWeightBlue = addWeightIntBlue / 10;
double addWeightRed = addWeightIntRed / 10;
int deRepeat = 1;
int houghThreshold = 10;
int radius = 1;
int counter = 0;
double actualBrightness = 0;
int trackbarBrightness = 0;
double actualContrast = 0;
int trackbarContrast = 0;
int thresholdValue = 0;

void gpuDilateAndErode(gpu::GpuMat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc); //prototype this one too

class gpuClass{
public: 
void gpuProcess( Mat &inProcess, Mat &outProcess, vector<Vec3f> &circlesData){
	Mat displayImageCPU; //create this for showing the filtered image
	gpuInFrame.upload(inProcess); //upload frame to GPU

	gpu::split(gpuInFrame, RGBchannels); //split image into 3 channels
	addWeightBlue = addWeightIntBlue / 10; //take value from slider and convert to double
	addWeightRed = addWeightIntRed / 10;
	gpu::addWeighted(RGBchannels[0],addWeightBlue,RGBchannels[2],addWeightRed,0,BlueAndRed); //add the red and blue channels
	gpu::subtract(RGBchannels[1],BlueAndRed,gpuOutFrame); //subtract the red and blue from the green

	gpu::threshold(gpuOutFrame,gpuOutFrame,thresholdValue,255,0);

	gpuDilateAndErode(gpuOutFrame,dilateSize,erodeSize, deRepeat); //dilate and erode on gpu
	gpuOutFrame.download(displayImageCPU);
	imshow("filtered image", displayImageCPU); //show filtered image
	waitKey(5);

	vector<Vec3f> cpuCirclesData; //create cpu matrix for circles data
	gpu::HoughCircles(gpuOutFrame, gpuCirclesData, CV_HOUGH_GRADIENT,1, 60, 100, 10, 1, 20); //run houghCircles on gpu
	gpu::HoughCirclesDownload(gpuCirclesData,cpuCirclesData); //download houghcirlces data to cpu
	for( size_t i = 0; i < cpuCirclesData.size(); i++ )
	{
		Point center(cvRound(cpuCirclesData[i][0]), cvRound(cpuCirclesData[i][1]));
		int radius = cvRound(cpuCirclesData[i][2]);
		// draw the circle center
		circle( outProcess, center, 3, Scalar(0,128,128), -1, 8, 0 );
		// draw the circle outline
		circle( outProcess, center, radius, Scalar(0,128,128), 3, 8, 0 );
		
	}

	//GaussianBlur( frame, out, Size(5,5), 3, 3);
	//imshow("output", gray); //show the frame in "MyVideo" window
	//imshow("output",temp ); //show the frame in "MyVideo" window

	circlesData = cpuCirclesData;
}
private:
gpu::GpuMat gpuInFrame, gpuOutFrame,BlueAndRed; //create gpu matricies to store the image
	gpu::GpuMat gpuCirclesData; //create gpu matrix for cirlces data
	vector <gpu::GpuMat> RGBchannels; //create vector of gpu matricies to store RGB channels

};


void dilateAndErode(Mat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc){

	//this function dilates and erodes the image, making shapes more defined and removing noise
	// function below takes the elementSize and gets a structuring element. bigger element means more erosion
	if (erodeSizeFunc == 0){ erodeSizeFunc = 1; } //checks if erode size = 0 and sets it to 1
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(erodeSizeFunc,erodeSizeFunc));
	Mat dilateElement = getStructuringElement( MORPH_RECT, Size(dilateSizeFunc,dilateSizeFunc));
	for(int i = 0; i < deRepeatFunc; i++){
		erode(input, input, erodeElement);
		dilate(input, input, dilateElement);
	}
}
void gpuDilateAndErode(gpu::GpuMat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc){
	//this function dilates and erodes the image, making shapes more defined and removing noise
	// function below takes the elementSize and gets a structuring element. bigger element means more erosion
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(erodeSizeFunc,erodeSizeFunc));
	Mat dilateElement = getStructuringElement( MORPH_RECT, Size(dilateSizeFunc,dilateSizeFunc));

	for(int i = 0; i < deRepeatFunc; i++){
		gpu::erode(input, input, erodeElement);
		gpu::dilate(input, input, dilateElement);
	}
}

void displayCircles(VideoCapture cap, vector<Vec3f> &circlesData,gpuClass &g){
	clock_t start = clock();
	Mat inputFrame, outputFrame; //creating variables to store image
	bool frameIsRead = cap.read(inputFrame); //reads a frame from video and stores boolean value if read correctly
	if (!frameIsRead) //check if frame was read
	{
		cout << "video was not read successfully" << endl;
	}

	pyrDown(inputFrame,inputFrame ); //scale image down, makes it easier to process
	outputFrame = inputFrame; // make a copy for output
	counter++;
	if(gpu::getCudaEnabledDeviceCount() != 0){ //checks if a CUDA device is availible
		g.gpuProcess(inputFrame, outputFrame, circlesData); // calls for the GPU to process the frame
	}
	//convert to RGB before displaying output
	imshow("final image", outputFrame);
	clock_t t = clock() - start;
	double secondT = (float(t))/CLOCKS_PER_SEC;
	//cout << secondT << endl;
}

void framesPerSecond(int counter, clock_t startTime)
{
	clock_t t;
	t = clock() - startTime;
	double secondT = (float(t))/CLOCKS_PER_SEC;
	double framesPerSec = counter/secondT;
	cout << framesPerSec << endl;
}

//Point of no return


/* Basic shell of a TCP/IP server which can run on Linux (or Windows
* with Cygwin or similar).  At this point it only accepts two
* commands = 0x00000000 closes the current connection, 0x00000001
* requests a read of data. That data is 3 4-byte values
*
* The server is single-threaded, which means it can only handle one
* connection. That's not going to be a problem unless the client
* disconnects unexpectedly and leaves the server hung. To fix this
* the server is coded to time out after 10 seconds of inactivity.
* I'd expect that our use will be polling for data way more frequently
* than that, so this is long enough to indicate a real problem.
*/


int main()
{
	gpuClass g;
	int dilateSize = 1;
	//open the video
	VideoCapture cap(0);
	//checks if the video opened
	if ( !cap.isOpened() ){
		cout << "video was not opened successfully." <<endl;
		return -1;
	}
	namedWindow("adjustments",0); //create a new windows for adjusting some variables during runtime
	createTrackbar("erode element size(pixels)","adjustments", &erodeSize, 10); //slider for erode size
	createTrackbar("dilate element size(pixels)","adjustments", &dilateSize, 10); //slider for dilate size
	createTrackbar("blue channel weight(x10)","adjustments", &addWeightIntBlue, 100); //blue channel weight slider
	createTrackbar("red channel weight(x10)","adjustments", &addWeightIntRed, 100); //red channel weight slider
	createTrackbar("dilate and erode","adjustments", &deRepeat, 10); //times to repeat dilate and erode
	createTrackbar("hough threshold","adjustments", &houghThreshold, 20); //threshold for hough circles detection
	createTrackbar("brightness","adjustments", &trackbarBrightness, 100);
	createTrackbar("contrast","adjustments", &trackbarContrast, 100);
	createTrackbar("threshold value","adjustments", &thresholdValue, 255);
	// loop that runs for every frame until stopped

	while (true)
	{
		
		actualBrightness = trackbarBrightness;
		actualBrightness /= 100;
		actualContrast = trackbarContrast;
		actualContrast /= 100;		
		cap.set(CV_CAP_PROP_BRIGHTNESS, actualBrightness);
		cap.set(CV_CAP_PROP_CONTRAST, actualContrast);
		vector<Vec3f> xAndY;
		displayCircles(cap, xAndY, g);
					
	}
	return 0;
}



