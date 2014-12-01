#include "opencv2/gpu/gpu.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

int erodeSize = 1;
int dilateSize = 1;
int addWeightIntBlue = 10;
int addWeightIntGreen = 10;
double addWeightBlue = addWeightIntBlue / 10;
double addWeightGreen = addWeightIntGreen / 10;
int deRepeat = 1;
int houghThreshold = 10;

void dilateAndErode(Mat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc); //prototype this function so that it can be called in cpuProcess
void gpuDilateAndErode(gpu::GpuMat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc); //prototype this one too



void cpuProcess(Mat &inProcess, Mat &outProcess){ //this function is called in main.
	Mat BlueAndGreen; 
	vector <Mat> RGBchannels; //create an array of matricies to store channels
	split(inProcess, RGBchannels); //split image into BGR channels
	addWeightBlue = addWeightIntBlue / 10; //take value from slider and convert to double
	addWeightGreen = addWeightIntGreen / 10;
	addWeighted(RGBchannels[0],addWeightBlue,RGBchannels[1],addWeightGreen,0,BlueAndGreen); //add blue and green
	subtract(RGBchannels[2],BlueAndGreen,inProcess); //subtract from red
	dilateAndErode(inProcess,dilateSize,erodeSize, deRepeat);
	//time to dilate and erode! change values to adjust
	//show output for debugging puposes
	imshow("filtered image", inProcess);
	//create a vector that is going to store the circles
	vector<Vec3f> circlesData;
	//find circles
	//hough circles arguments are:
	//input, circles vector,
	//method, - don't touch this one
	//no idea what this does, - leave at 1
	//min_dist, = Minimum distance between detected centers
	//param_1, Upper threshold for the internal Canny edge detector
	//param_2, Threshold for center detection.
	//min_radius, Minimum radius to be detected. 
	//max_radius
	//radius significantly alters processing time 
	HoughCircles(inProcess, circlesData, CV_HOUGH_GRADIENT, 1, 200, 100,houghThreshold, 0, 0); 
	// going to start a loop for each detected circle to draw 
	for (size_t i=0; i < circlesData.size(); i++){
		Point center(cvRound(circlesData[i][0]), cvRound(circlesData[i][1])); //read the center point x and y values
		int radius = cvRound(circlesData[i][2]);
		//read the radius

		//draw the circle center
		circle(outProcess, center, 3, Scalar(0,128,128), -1, 8, 0);

		//draw the circle outline
		circle(outProcess, center, radius, Scalar(0,128,128),3,8,0);
	}

}


void gpuProcess( Mat &inProcess, Mat &outProcess){
	Mat displayImageCPU; //create this for showing the filtered image
	gpu::GpuMat gpuInFrame, gpuOutFrame,BlueAndGreen; //create gpu matricies to store the image
	gpuInFrame.upload(inProcess); //upload frame to GPU
	vector <gpu::GpuMat> RGBchannels; //create vector of gpu matricies to store RGB channels
	gpu::split(gpuInFrame, RGBchannels); //split image into 3 channels
	addWeightBlue = addWeightIntBlue / 10; //take value from slider and convert to double
	addWeightGreen = addWeightIntGreen / 10;
	gpu::addWeighted(RGBchannels[0],addWeightBlue,RGBchannels[1],addWeightGreen,0,BlueAndGreen); //add the green and blue channels
	gpu::subtract(RGBchannels[2],BlueAndGreen,gpuOutFrame); //subtract the green and blue from the red
	gpuDilateAndErode(gpuOutFrame,dilateSize,erodeSize, deRepeat); //dilate and erode on gpu		
	gpuOutFrame.download(displayImageCPU);
	imshow("filtered image", displayImageCPU); //show filtered image 
	gpu::GpuMat gpuCirclesData; //create gpu matrix for cirlces data
	vector<Vec3f> cpuCirclesData; //create cpu matrix for circles data
	gpu::HoughCircles(gpuOutFrame, gpuCirclesData, CV_HOUGH_GRADIENT,1, 200, 100, 10, 0, 0); //run houghCircles on gpu
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
}



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

int main(int argc, char* argv[]){

int dilateSize = 1;
//open the video
VideoCapture inputVideo("juggle.mov");
//checks if the video opened
if ( !inputVideo.isOpened() ){
	cout << "video was not opened successfully." <<endl;
	return -1;
} 
namedWindow("adjustments",0); //create a new windows for adjusting some variables during runtime
createTrackbar("erode element size(pixels)","adjustments", &erodeSize, 10); //slider for erode size
createTrackbar("dilate element size(pixels)","adjustments", &dilateSize, 10); //slider for dilate size
createTrackbar("blue channel weight(x10)","adjustments", &addWeightIntBlue, 100); //blue channel weight slider
createTrackbar("green channel weight(x10)","adjustments", &addWeightIntGreen, 100); //green channel weight slider
createTrackbar("dilate and erode","adjustments", &deRepeat, 10); //times to repeat dilate and erode
createTrackbar("hough threshold","adjustments", &houghThreshold, 20); //threshold for hough circles detection
// loop that runs for every frame until stopped
while(1)
{

Mat inputFrame, outputFrame; //creating variables to store image
bool frameIsRead = inputVideo.read(inputFrame); //reads a frame from video and stores boolean value if read correctly
if (!frameIsRead) //check if frame was read 
{
	cout << "video was not read successfully" << endl;
	break; //break loop if not read
}

pyrDown(inputFrame,inputFrame ); //scale image down, makes it easier to process
outputFrame = inputFrame; // make a copy for output
if(gpu::getCudaEnabledDeviceCount() != 0){ //checks if a CUDA device is availible
gpuProcess(inputFrame, outputFrame); // calls for the GPU to process the frame
}else{
cpuProcess(inputFrame, outputFrame); //calls for the CPU to process the frame
}
//convert to RGB before displaying output
imshow("final image", outputFrame); 
//waits for escape key to be pressed, this has to be changed 
if(waitKey(100) == 2){
	cout << "esc key pressed, exiting program" << endl;
	break;
	}
    }

}

