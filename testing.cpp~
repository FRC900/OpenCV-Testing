#include "opencv2/highgui/highgui.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace cv;
using namespace std;

const string trackbarWindowName = "Adjust me";
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

void on_trackbar( int, void* ){
	
}

void createTrackbarsWindow(){
	namedWindow(trackbarWindowName,0);
	
	char TrackbarName[50];
	
	/*
	sprintf( TrackbarName, "H_MIN", H_MIN);
	sprintf( TrackbarName, "H_MAX", H_MAX);
	sprintf( TrackbarName, "S_MIN", S_MIN);
	sprintf( TrackbarName, "S_MAX", S_MAX);
	sprintf( TrackbarName, "V_MIN", V_MIN);
	sprintf( TrackbarName, "V_MAX", V_MAX);
	*/

	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar);
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar);
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar);
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar);
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar);
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar);
}

void gpuMorphOps(gpu::GpuMat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_ELLIPSE,Size(12,12));

	//gpu::erode(thresh,thresh,erodeElement);
	//gpu::erode(thresh,thresh,erodeElement);
	//you have to comment these back in for it to work
	//gpu::dilate(thresh,thresh,dilateElement);
	//gpu::dilate(thresh,thresh,dilateElement);

}
void morphOps(Mat &thresh){

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_ELLIPSE,Size(12,12));

	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);

	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);

}

void displayGPUImage(char* name, gpu::GpuMat mat){
	Mat temp;
	mat.download(temp);
	imshow(name, temp);
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
	//gpu::cvtColor(gpuTemp, gpuGray, CV_BGR2HSV);

	//gpuGray.download(temp);
	
	//inRange(temp, Scalar(165,105,170),Scalar(230,256,205),out);
	vector <gpu::GpuMat> channels;
	gpu::split(gpuFrame, channels);
	gpu::add(channels[0],channels[1],gpuTemp);
	gpu::subtract(channels[2],gpuTemp,gpuFrame);
	//gpuOut.upload(out);
	displayGPUImage("GPU out", gpuFrame);
	gpuMorphOps(gpuFrame);
	displayGPUImage("GPU out after circles", gpuFrame);



	gpu::GpuMat gpuCircles;
	vector<Vec3f> cpuCircles;
	gpu::HoughCircles(gpuFrame, gpuCircles, CV_HOUGH_GRADIENT, 1, frame.rows/20, 20, 10, 1, frame.rows/10);
	gpu::HoughCirclesDownload(gpuCircles,cpuCircles);
	for( size_t i = 0; i < cpuCircles.size(); i++ )
	{
		Point center(cvRound(cpuCircles[i][0]), cvRound(cpuCircles[i][1]));
		int radius = cvRound(cpuCircles[i][2]);
		// draw the circle center
		circle( frame, center, 3, Scalar(0,128,128), -1, 8, 0 );
		// draw the circle outline
		circle( frame, center, radius, Scalar(0,128,128), 3, 8, 0 );
	}

	//GaussianBlur( frame, out, Size(5,5), 3, 3);
	//imshow("output", gray); //show the frame in "MyVideo" window
	//imshow("output",temp ); //show the frame in "MyVideo" window
	imshow("GPU in", frame);
	//outputVideo.write(temp);




}

int main(int argc, char* argv[])
{
    VideoCapture cap("juggle.mov"); // open the video file for reading

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the video file" << endl;
         return -1;
    }

		createTrackbarsWindow();

		//double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
		//double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

		//Size frameSize(static_cast<int>(dWidth)/2, static_cast<int>(dHeight)/2);

		//VideoWriter outputVideo("temp.mpg",CV_FOURCC('P','I','M','1'), 20, frameSize, true);
	
    while(1)
    {
	GPUprocess(cap);		
        /*Mat frame;
	Mat out, temp, gray;
        Mat lotsOfCircles;
        bool bSuccess = cap.read(frame); // read a new frame from video
				gpu::GpuMat gpuFrame(frame);
				//cap.set (CV_CAP_PROP_POS_FRAMES,200);
         if (!bSuccess) //if not success, break loop
        {
                        cout << "Cannot read the frame from video file" << endl;
                       break;
        }
				
				//cv::GaussianBlur( frame, out, Size(5,5), 3, 3);
				pyrDown(frame, temp);				
				cvtColor(temp, gray, CV_BGR2HSV);
        //imshow("MyVideo", frame); //show the frame in "MyVideo" window

				//inRange(gray, Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),out);
				//imshow("output", gray);
				inRange(gray, Scalar(165,105,170),Scalar(230,256,205),out);
				//imshow("output", out);
				morphOps(out);
				imshow("output", out);				

				vector<Vec3f> circles;

				HoughCircles(out, circles, CV_HOUGH_GRADIENT, 1, out.rows/20, 20, 10, 0, 0 );
        //normalize(out,out,0,255,NORM_MINMAX);
        //out.convertTo(lotsOfCircles,CV_32F);
				//cvtColor(out,lotsOfCircles , CV_HSV2BGR);
				for( size_t i = 0; i < circles.size(); i++ ){
					Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
					int radius = cvRound(circles[i][2]);
					// draw the circle center
					circle( temp, center, 3, Scalar(0,128,128), -1, 8, 0 );
					// draw the circle outline
					circle( temp, center, radius, Scalar(0,128,128), 3, 8, 0 );
				}

				//GaussianBlur( frame, out, Size(5,5), 3, 3);
        //imshow("output", gray); //show the frame in "MyVideo" window
				//imshow("output",temp ); //show the frame in "MyVideo" window
				//imshow("input", temp);
				//outputVideo.write(temp);
        */

				if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
       {
                cout << "esc key is pressed by user" << endl; 
                break; 
       }
    }

    return 0;

}
