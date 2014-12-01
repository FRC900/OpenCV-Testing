#include <iostream>
#include <vector>
#include<stdio.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

char mouseController[] = "Raw Video";
char objWindow[] = "Object Window";
char scribbleWindow[] = "Scribble Window";
char resultWindow[] = "Mouse Controller";
int main() {
 static int posX, posY, lastX, lastY; //To hold the X and Y position of tracking color object
 int i, j;

  Mat rgbCameraFrames; //Matrix to hold Raw frames from Webcam in RGB color space
 Mat colorTrackingFrames; //Matrix to hold color filtered Frames in GRAY color space
 Mat resutantFrame; //Matrix to add RAW and user scribble Frames

  VideoCapture capture(0); //Used to access the default camera in your machine

  namedWindow(mouseController, CV_WINDOW_AUTOSIZE); //Can be used to change the window properties

  CvMoments colorMoment; //Structure to hold moments information and their order
 capture >> rgbCameraFrames; //Simply by overloading the operator we copy the Camera frames to our rgbCameraFrames object
 assert(rgbCameraFrames.type() == CV_8UC3);
 //Checking whether the color space is in RGB space

  Mat *scribbleFrame = new Mat(rgbCameraFrames.rows, rgbCameraFrames.cols,
   CV_8UC3); //A matrix to hold the user scribble on the screen

  while (1) {
  capture >> rgbCameraFrames; //Each frames is copied to our rgbCameraFrames object

   GaussianBlur(rgbCameraFrames, colorTrackingFrames, Size(11, 11), 0, 0); //Just a filter to reduce the noise
//           B  G  R     B   G    R
  inRange(colorTrackingFrames, Scalar(0, 0, 115), Scalar(50, 50, 255),// Adjust the scalar values for different color
    colorTrackingFrames); //We make RED color pixels to white and other colors to black

   colorMoment = moments(colorTrackingFrames); //We give the binary converted frames for calculating the moments
  double moment10 = cvGetSpatialMoment(&colorMoment, 1, 0); //Sum of X coordinates of all white pixels
  double moment01 = cvGetSpatialMoment(&colorMoment, 0, 1); //Sum of Y coordinates of all white pixels
  double area = cvGetCentralMoment(&colorMoment, 0, 0); //Sum of all white color pixels
  printf("1. x-Axis moments %f  y-Axis moments %f  Area of the moment  %f\n",
    moment10, moment01, area);
  printf("2. x movement %f  y movement %f \n\n", moment10 / area,
    moment01 / area);
//  From terminal :
//  1. x-Axis moments 760645620.000000  y-Axis moments 631169625.000000  Area of the moment  1994355.000000
//  2. x movement 381.399310  y movement 316.478072

   lastX = posX;
  lastY = posY;

   posX = (moment10 / area);
  posY = moment01 / area; //Simple logic which you can understand

   if (posX > 0 && posY > 0 && lastX > 0 && lastY >> 0) {
   line(*scribbleFrame, cvPoint(posX, posY), cvPoint(lastX, lastY),
     cvScalar(0, 255, 255), 1); //To draw a continuous stretch of lines
   line(rgbCameraFrames, cvPoint(posX, posY), cvPoint(lastX, lastY),
     cvScalar(0, 255, 255), 5); //To draw a yello point on the center of the colored object
              //cvPoint is used to create a Point data type which holds the pixel location
  }

   imshow(scribbleWindow, *scribbleFrame);
  imshow(mouseController, rgbCameraFrames);
  imshow(objWindow, colorTrackingFrames);
  add(rgbCameraFrames, *scribbleFrame, resutantFrame); //Add two Matrix of the same size
  imshow(resultWindow, resutantFrame);
  waitKey(1); // OpenCV way of adding a delay, generally used to get a Key info.
 }

  return 0;
}
