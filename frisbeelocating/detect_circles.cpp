#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

#include "detect_circles.hpp"

#define DISPLAY_OUTPUT

using namespace std;
using namespace cv;

BaseHoughCircles::BaseHoughCircles(const char *filename)
{
   video.open(filename);
#if 0
   video.open(0);
   video.set(CV_CAP_PROP_FPS, 50);
   video.set(CV_CAP_PROP_FRAME_WIDTH, 640);
   video.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
#endif
   if(!video.isOpened()){
      cerr << "Cannot open the file" << endl;
   }

#if 1
   H_MIN = 151;
   H_MAX = 173;
   S_MIN = 105;
   S_MAX = 215;
   V_MIN = 170;
   V_MAX = 235;
#else
   H_MIN = 82;
   H_MAX = 173;
   S_MIN = 60;
   S_MAX = 255;
   V_MIN = 170;
   V_MAX = 235;
#endif
#ifdef DISPLAY_OUTPUT
   createHSVTrackbarsWindow();
#endif
}
void BaseHoughCircles::createHSVTrackbarsWindow(void){
   string trackbarWindowName = "Adjust HSV Parameters";
   namedWindow(trackbarWindowName,cv::WINDOW_AUTOSIZE);

   createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, 179, NULL);
   createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, 179, NULL);
   createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, 255, NULL);
   createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, 255, NULL);
   createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, 255, NULL);
   createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, 255, NULL);
}

GpuHoughCircles::GpuHoughCircles(const char *filename) : BaseHoughCircles(filename)
{
   gpuSplitImage.resize(3);
   outputDataAvailable = false;
   Mat foo;
   video.read(foo);
   for (int i = 0; i < 1; i++)
   {
      cerr << "Init " << i << " " << foo.rows << "x" << foo.cols << endl;
      cudaMemIn[i] = cv::gpu::CudaMem(foo, cv::gpu::CudaMem::ALLOC_PAGE_LOCKED);
      //cudaMemOut[i].create(1, 50, CV_32FC3, cv::gpu::CudaMem::ALLOC_PAGE_LOCKED);
   }

   cudaMemIndex = 0;
}

void GpuHoughCircles::cvOpen(const cv::gpu::GpuMat &gpuImageIn, cv::gpu::GpuMat &gpuMorphOpsOut, cv::gpu::Stream &gpuStream)
{
   Mat erodeElement (getStructuringElement( cv::MORPH_RECT,cv::Size(3,3)));
   //dilate with larger element to make sure object is nicely visible
   Mat dilateElement(getStructuringElement( cv::MORPH_ELLIPSE,cv::Size(11,11)));

   cv::gpu::erode(gpuImageIn, gpuMorphOpsTemp, erodeElement, cv::Point(-1,-1), 2);
   cv::gpu::dilate(gpuMorphOpsTemp, gpuMorphOpsOut, dilateElement, cv::Point(-1,-1), 2);

#if 0
   cv::gpu::HoughCircles(gpuMorphOpsOut, gpuHoughCirclesOut, gpuHoughCirclesBuffer, CV_HOUGH_GRADIENT, 1, 35, 20, 10, 2, 19 );
   cv::gpu::HoughCirclesDownload(gpuHoughCirclesOut, circleList);
//#else
   Mat localImage;
   gpuMorphOpsOut.download(localImage);
   cv::HoughCircles(localImage, circleList, CV_HOUGH_GRADIENT, 1, 35, 20, 10, 0, 0 );
#endif
}

void GpuHoughCircles::generateThreshold(const cv::gpu::GpuMat &gpuImageIn, cv::gpu::GpuMat &gpuImageOut, cv::gpu::Stream &gpuStream)
{

   cv::gpu::cvtColor(gpuImageIn, gpuThresholdLocalImage, CV_BGR2HSV, 0);

   cv::gpu::split(gpuThresholdLocalImage, gpuSplitImage);

   int max[3] = {H_MAX, S_MAX, V_MAX};
   int min[3] = {H_MIN, S_MIN, V_MIN};
   for (size_t i = 0; i < gpuSplitImage.size(); i++)
   {
      cv::gpu::compare(gpuSplitImage[i], min[i], gpuSplitImageGE, cv::CMP_GE);
      cv::gpu::compare(gpuSplitImage[i], max[i], gpuSplitImageLE, cv::CMP_LE);
      cv::gpu::bitwise_and(gpuSplitImageGE, gpuSplitImageLE, gpuSplitImage[i]);
   }
   cv::gpu::bitwise_and(gpuSplitImage[0], gpuSplitImage[1], gpuImageOut);
   cv::gpu::bitwise_and(gpuSplitImage[2], gpuImageOut, gpuImageOut);
}

int  GpuHoughCircles::detectCircles(vector<Vec3f> &circlesData)
{
   int ret = 0;
   // Enqueue a list of things to do on the GPU, all tagged with gpuStream
   cv::gpu::Stream gpuStream;
   gpuFrame.upload(cudaMemIn[cudaMemIndex].createMatHeader());
   cv::gpu::pyrDown(gpuFrame, gpuPyrDownFrame);
   generateThreshold(gpuPyrDownFrame, gpuThresholdImageOut, gpuStream);
   cvOpen(gpuThresholdImageOut, gpuMorphOpsOut, gpuStream);
   cv::gpu::HoughCircles(gpuMorphOpsOut, gpuHoughCirclesOut, gpuHoughCirclesBuffer, CV_HOUGH_GRADIENT, 1, 35, 20, 10, 2, 19 );
   cv::gpu::HoughCirclesDownload(gpuHoughCirclesOut, circlesData);
   
   // While those are running, work on the CPU side to grab the next
   // video frame and run HoughCircles on the previously GPU-filtered 
   // output frame
   //cudaMemIndex ^= 1;

   //double tick = cvGetTickCount();
   Mat localFrame = cudaMemIn[cudaMemIndex].createMatHeader();
   bool readVideoSuccess = video.read(localFrame);
   if (!readVideoSuccess){
      cout << "Cannot read from file" << endl;
      ret = -1;
   }
   //tick = (double(cvGetTickCount()) - tick) / cvGetTickFrequency();
   //cout << "video.read() " << tick <<endl;

#if 0
   tick = cvGetTickCount();
   if (outputDataAvailable)
   {
      cv::gpu::GpuMat localCirclesIn = cudaMemOut[cudaMemIndex].createGpuMatHeader();
      cv::gpu::HoughCircles(localCirclesIn, gpuHoughCirclesOut, gpuHoughCirclesBuffer, CV_HOUGH_GRADIENT, 1, 35, 20, 10, 2, 19 );
      //cout << gpuHoughCirclesOut.rows << "x" << gpuHoughCirclesOut.cols << endl;
      cv::gpu::HoughCirclesDownload(gpuHoughCirclesOut, circlesData);
   }
   tick = (double(cvGetTickCount()) - tick) / cvGetTickFrequency();
   cout << "GpuHoughCircles " << tick <<endl;



   tick = cvGetTickCount();
   gpuStream.waitForCompletion();
   tick = (double(cvGetTickCount()) - tick) / cvGetTickFrequency();
   cout << "waitForCompletion " << tick <<endl;
   outputDataAvailable = true;
#endif
#ifdef DISPLAY_OUTPUT
   Mat  displayTemp = cudaMemIn[cudaMemIndex].createMatHeader();
   pyrDown(displayTemp, displayTemp);
   for(size_t i = 0; i < circlesData.size(); i++ )
   {
      cv::Point center(cvRound(circlesData[i][0]), cvRound(circlesData[i][1]));
      int radius = cvRound(circlesData[i][2]);
      // draw the circle center
      circle(displayTemp, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
      // draw the circle outline
      circle(displayTemp, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
   }
   Mat output;
   gpuThresholdImageOut.download(output);
   //cv::imshow("hsv output", output); //show the input in "myvideo" window
   cv::imshow("rgb output", displayTemp);  //show the displayTemp in "myvideo" window
   waitKey(1);
#endif
   return ret;
}

CpuHoughCircles::CpuHoughCircles(const char *filename) : BaseHoughCircles(filename)
{
}

void CpuHoughCircles::makeCircles(const Mat &cpu_image, std::vector<cv::Vec3f> &circle_list)
{
   Mat erodeElement (getStructuringElement( cv::MORPH_RECT,cv::Size(3,3)));
   //dilate with larger element so make sure object is nicely visible
   Mat dilateElement(getStructuringElement( cv::MORPH_ELLIPSE,cv::Size(11,11)));

   Mat local_image;
   cv::erode(cpu_image, local_image, erodeElement, cv::Point(-1,-1), 2);
   cv::dilate(local_image, local_image, dilateElement, cv::Point(-1,-1), 2);
   cv::HoughCircles(local_image, circle_list, CV_HOUGH_GRADIENT, 1, 35, 20, 10, 0, 0 );
}
void CpuHoughCircles::generateThreshold(const Mat &imageIn, Mat &imageOut)
{
   Mat localImage;
   Mat tmpImage;

   //cv::GaussianBlur( frame, out, Size(5,5), 3, 3);
   //cv::pyrDown(imageIn, hsv);				
   //cv::pyrDown(imageIn, imageOut);				
   cv::cvtColor(imageIn, localImage, CV_BGR2HSV);
   //imshow("MyVideo", cpu_image); //show the frame in "MyVideo" window
   //Mat cpuGray(gpuGray);
   std::vector<Mat> splitImage(3);
#if 0
   std::vector<Mat> biggerThanMin(3);
   std::vector<Mat> smallerThanMax(3);
#endif
   cv::split(localImage, splitImage);
   // Redo using loop similar to GPU code
#if 0
   cv::compare(splitImage[0], H_MIN, biggerThanMin[0], cv::CMP_GE);
   cv::compare(splitImage[0], H_MAX, smallerThanMax[0], cv::CMP_LE);
   cv::compare(splitImage[1], S_MIN, biggerThanMin[1], cv::CMP_GE);
   cv::compare(splitImage[1], S_MAX, smallerThanMax[1], cv::CMP_LE);
   cv::compare(splitImage[2], V_MIN, biggerThanMin[2], cv::CMP_GE);
   cv::compare(splitImage[2], V_MAX, smallerThanMax[2], cv::CMP_LE);
   imageOut = biggerThanMin[0] & smallerThanMax[0] &
      biggerThanMin[1] & smallerThanMax[1] &
      biggerThanMin[2] & smallerThanMax[2];
#endif
   Mat compare_out;

   imageOut.create(splitImage[0].rows, splitImage[0].cols, splitImage[0].type());
   imageOut.setTo(cv::Scalar::all(0xff));
   int max[3] = {H_MAX, S_MAX, V_MAX};
   int min[3] = {H_MIN, S_MIN, V_MIN};
   for (size_t i = 0; i < splitImage.size(); i++)
   {
      cv::compare(splitImage[i], min[i], compare_out, cv::CMP_GE);
      cv::bitwise_and(compare_out, imageOut, imageOut);
      cv::compare(splitImage[i], max[i], compare_out, cv::CMP_LE);
      cv::bitwise_and(compare_out, imageOut, imageOut);
   }

#if 0
   cv::inRange(localImage, cv::Scalar(H_MIN,S_MIN,V_MIN),cv::Scalar(H_MAX,S_MAX,V_MAX),imageOut);

   Mat displayCompare;

   cv::compare(tmpImage, imageOut, displayCompare, cv::CMP_EQ);
   cv::imshow("Compare image", displayCompare); //show the frame in "MyVideo" window
#endif
   //cv::inRange(cpuGray, cv::Scalar(160,105,170),cv::Scalar(170,210,205),imageOut);
   //morphOps(imageOut);
   //imageOut = Mat(gpuGray);
   //imageOut = Mat(gpuImageOut);
}

int CpuHoughCircles::detectCircles(vector<Vec3f> &circlesData)
{
   Mat frame, thresholdOutput;
   bool readVideoSuccess = video.read(frame);
   if (!readVideoSuccess){
      cout << "Cannot read from file" << endl;
      circlesData.clear();
      return -1;
   }

   pyrDown(frame, frame);
   generateThreshold(frame, thresholdOutput);
   makeCircles(thresholdOutput, circlesData);

#ifdef DISPLAY_OUTPUT
   for(size_t i = 0; i < circlesData.size(); i++ )
   {
      cv::Point center(cvRound(circlesData[i][0]), cvRound(circlesData[i][1]));
      int radius = cvRound(circlesData[i][2]);
      // draw the circle center
      circle(frame, center, 3, cv::Scalar(0,255,0), -1, 8, 0);
      // draw the circle outline
      circle(frame, center, radius, cv::Scalar(0,0,255), 3, 8, 0);
   }
   cv::imshow("HSV output", thresholdOutput); //show the frame in "MyVideo" window
   cv::imshow("RGB output", frame); //show the frame in "MyVideo" window
   waitKey(1);
#endif
   return 0;
}
