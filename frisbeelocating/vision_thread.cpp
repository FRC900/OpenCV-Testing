
#include <sys/time.h>

#include "vision_thread.hpp"
#include "detect_circles.hpp"
#include "threadsafe_buffer.hpp"

using namespace cv;
using namespace std;

double getTimeAsDouble(void)
{
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv.tv_sec + tv.tv_usec/1000000.;
}

void printFPS(double frameCount, double elapsedTime)
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

   // list of (x,y,radius) circles identified
   vector<Vec3f> vec;

   VisionBuf buf;

   // Class used for image detection
   BaseHoughCircles *houghCircles;

   unsigned frameCount = 0;
   double startTime = getTimeAsDouble();
   while (1)
   {
      if (gpu::getCudaEnabledDeviceCount() > 0)
	 houghCircles = new GpuHoughCircles("juggle.mov");
      else
	 houghCircles = new CpuHoughCircles("juggle.mov");

      // Each call to detectCircles returns circle data for
      // one frame.  
      while (houghCircles->detectCircles(vec) != -1)
      {
	 buf.clear();
	 appendToBuffer(buf, vec.size());
	 for (size_t i = 0; i < vec.size(); i++)
	    for (size_t j = 0; j < 3; j++)
	       appendToBuffer(buf, unsigned(cvRound(vec[i][j])));
	 tsBuffer->Update(buf);
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
      // Print FPS at end of file as well
      printFPS(getTimeAsDouble() - startTime, frameCount - skipInitialFrames);

      delete houghCircles;
   }
   return NULL;
}

