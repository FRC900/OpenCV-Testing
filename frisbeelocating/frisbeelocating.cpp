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

#include <sys/time.h>

#include "vision_thread.hpp"
#include "threadsafe_buffer.hpp"
using namespace cv;
using namespace std;

int erodeSize = 5;
int dilateSize = 1;
int deRepeat = 1;
int houghThreshold = 10;
int radius = 1;
int counter = 0;
double actualBrightness = 0;
int trackbarBrightness = 1;
double actualContrast = 0;
int trackbarContrast = 50;
int thresholdValueGreen = 88;
int thresholdValueRed = 155;
int circleSize = 30;
int minCircularity = 70;
int field = 60;

void gpuDilateAndErode(gpu::GpuMat &input, gpu::GpuMat &output, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc); //prototype this one too

class gpuClass{
public: 
void gpuProcess( Mat &inProcess, Mat &outProcess, vector<Vec3f> &circlesData){
	Mat displayImage1;
	Mat displayImage2; //create this for showing the filtered image
	cv::gpu::GpuMat thresh2;
	gpuInFrame.upload(inProcess); //upload frame to GPU

	gpu::split(gpuInFrame, RGBchannels); //split image into 3 channels
	gpu::threshold(RGBchannels[2],RGBchannels[2],thresholdValueRed,255,0);	
	gpu::subtract(RGBchannels[1],RGBchannels[2],GreenMinus); //subtract the red from the green	
	gpu::threshold(GreenMinus,gpuOutFrame,thresholdValueGreen,255,0);

	gpuDilateAndErode(gpuOutFrame,gpuOutFrame,dilateSize,erodeSize, deRepeat); //dilate and erode on gpu
	GreenMinus.download(displayImage1);
	//imshow("GreenMinus", displayImage1); //show filtered image
	gpuOutFrame.download(displayImage2);
	Mat cpuFrame;
	vector< vector<Point> > foundContours;
	gpuOutFrame.download(cpuFrame);
	findContours(cpuFrame, foundContours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	double maxCircularity = minCircularity / 100.00;
	vector <Point> mostCircular;
	for(size_t i = 0; i < foundContours.size(); i++){
		vector <Point> currentContour;
		convexHull(foundContours[i], currentContour);
		double circularity = (4 * M_PI * contourArea(currentContour)) / (arcLength(currentContour,true) * arcLength(currentContour,true));
		if (circularity > maxCircularity && contourArea(currentContour) > circleSize){
			maxCircularity = circularity;
			mostCircular = currentContour;
		}
	}
	circlesData.clear();
	if(maxCircularity > minCircularity / 100.00){
		Moments M = moments( mostCircular, false);
		//cout << "most circularity: " << maxCircularity << endl;
		Point2f center(M.m10/M.m00 , M.m01/M.m00);
		int radius = arcLength(mostCircular, true) / (2 * M_PI);
		circle(outProcess, center, radius, Scalar(0,255,0), 10);
		circlesData.push_back(Vec3f(M.m10/M.m00 , displayImage2.cols , field));
	}	
	imshow("gpuOutFrame", displayImage2); //show filtered image
	waitKey(5);
	
}
private:
gpu::GpuMat gpuInFrame, gpuOutFrame,GreenMinus; //create gpu matricies to store the image
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
void gpuDilateAndErode(gpu::GpuMat &input, gpu::GpuMat &output, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc){
	//this function dilates and erodes the image, making shapes more defined and removing noise
	// function below takes the elementSize and gets a structuring element. bigger element means more erosion
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(erodeSizeFunc,erodeSizeFunc));
	Mat dilateElement = getStructuringElement( MORPH_RECT, Size(dilateSizeFunc,dilateSizeFunc));
	gpu::GpuMat dilateTemp;
	for(int i = 0; i < deRepeatFunc; i++){
		gpu::dilate(input, dilateTemp, dilateElement);
		gpu::erode(dilateTemp, output, erodeElement);
	}
}

void displayCircles(VideoCapture cap, vector<Vec3f> &circlesData,gpuClass &g){
	//clock_t start = clock();
	Mat inputFrame, outputFrame; //creating variables to store image
	bool frameIsRead = true;
	cap.read(inputFrame); //reads a frame from video and stores boolean value if read correctly
	if (!frameIsRead) //check if frame was read
	{
		cout << "video was not read successfully" << endl;
	}

	//pyrDown(inputFrame,inputFrame ); //scale image down, makes it easier to process
	//inputFrame = imread("Screenshot.png", CV_LOAD_IMAGE_UNCHANGED);
	outputFrame = inputFrame.clone(); // make a copy for output
	counter++;
	if(gpu::getCudaEnabledDeviceCount() != 0){ //checks if a CUDA device is availible
		g.gpuProcess(inputFrame, outputFrame, circlesData); // calls for the GPU to process the frame
	}
	//convert to RGB before displaying output
	//imshow("final image", outputFrame);
        //waitKey(5);
	//clock_t t = clock() - start;
	//double secondT = (float(t))/CLOCKS_PER_SEC;
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


double getTimeAsDouble(void)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec + tv.tv_usec/1000000.;
}

void printFPS(double elapsedTime, double frameCount)
{
   double fps = frameCount / elapsedTime;
   cout << "Processed " << frameCount << " frames in " << elapsedTime << " seconds. " << fps << " frames per second" << endl;
}

// Convert unsigned int into 4 individual bytes, push them 
void appendToBuffer(VisionBuf &buf, unsigned int val)
{
   buf.push_back(val >> 24);
   buf.push_back(val >> 16);
   buf.push_back(val >>  8);
   buf.push_back(val      );
}
const int skipInitialFrames = 10;

void *visionThread(void *data)
{
   // Buffer used to share data between threads
   TSBuffer< VisionBuf > *tsBuffer = ( TSBuffer< VisionBuf > *)data;
   VisionBuf buf;

   unsigned frameCount = 0;
   double startTime = getTimeAsDouble();
	gpuClass g;
	int dilateSize = 1;
	//open the video
	VideoCapture cap(0);
	//checks if the video opened
	if ( !cap.isOpened() ){
		cout << "video was not opened successfully." <<endl;
		return NULL;
	}
	namedWindow("adjustments",0); //create a new windows for adjusting some variables during runtime
	createTrackbar("erode element size(pixels)","adjustments", &erodeSize, 10); //slider for erode size
	createTrackbar("dilate element size(pixels)","adjustments", &dilateSize, 10); //slider for dilate size
	createTrackbar("dilate and erode","adjustments", &deRepeat, 10); //times to repeat dilate and erode
	createTrackbar("brightness","adjustments", &trackbarBrightness, 100);
	createTrackbar("contrast","adjustments", &trackbarContrast, 100);
	createTrackbar("circle size","adjustments", &circleSize, 1000);
	createTrackbar("green threshold value","adjustments", &thresholdValueGreen, 255);
	createTrackbar("red threshold value","adjustments", &thresholdValueRed, 255);
	createTrackbar("minimum circularity x 100","adjustments", &minCircularity, 100);
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

		buf.clear();
		appendToBuffer(buf, xAndY.size());
		for (size_t i = 0; i < xAndY.size(); i++)
		   for (size_t j = 0; j < 3; j++)
		   {
		      //cout << unsigned(cvRound(xAndY[i][j])) << " ";
		      appendToBuffer(buf, unsigned(cvRound(xAndY[i][j])));
		   }
		//cout << endl;
		tsBuffer->Update(buf);
		//for (size_t i = 0; i < buf.size(); i++)
		   //cout << (unsigned int)buf[i] << " "; 
		//cout << endl;
		// Update frame count - used for generating FPS display
		frameCount += 1;
		// Skip over the first few frames since OpenCV has a high
		// startup cost. Skipping the time for that startup gives
		// a more accurate reading of the steady-state framerate
		if (frameCount == skipInitialFrames)
		   startTime = getTimeAsDouble();
		// Print FPS every 60 frames
		else if (((frameCount - skipInitialFrames) % 60) == 0)
		{
		   printFPS(getTimeAsDouble() - startTime, frameCount - skipInitialFrames);
		   // reset frameCount so the start timer is reset on the next frame
		   frameCount = skipInitialFrames - 1;
		}
	}
	return NULL;
}



