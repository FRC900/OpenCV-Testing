#include "/usr/local/include/opencv2/gpu/gpu.hpp"
#include "/usr/local/include/opencv2/opencv.hpp"
#include "/usr/local/include/opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
int radius = 1;

void dilateAndErode(Mat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc); //prototype this function so that it can be called in cpuProcess
void gpuDilateAndErode(gpu::GpuMat &input, int dilateSizeFunc, int erodeSizeFunc, int deRepeatFunc); //prototype this one too



void cpuProcess(Mat &inProcess, Mat &outProcess, vector<Vec3f> &circlesData){ //this function is called in main.
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
		radius = cvRound(circlesData[i][2]);
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

void displayCircles(VideoCapture inputVideo, vector<Vec3f> &circlesData){
Mat inputFrame, outputFrame; //creating variables to store image
bool frameIsRead = inputVideo.read(inputFrame); //reads a frame from video and stores boolean value if read correctly
if (!frameIsRead) //check if frame was read
{
	cout << "video was not read successfully" << endl;
}

pyrDown(inputFrame,inputFrame ); //scale image down, makes it easier to process
outputFrame = inputFrame; // make a copy for output
if(gpu::getCudaEnabledDeviceCount() != 0){ //checks if a CUDA device is availible
//gpuProcess(inputFrame, outputFrame); // calls for the GPU to process the frame
}else{
cpuProcess(inputFrame, outputFrame, circlesData); //calls for the CPU to process the frame
}
//convert to RGB before displaying output
imshow("final image", outputFrame);
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

/* Create a TCP/IP socket and wait for a connection
 * Returns the new socket file descriptor to use
 * for subsequent read and write calls.
 */
int make_connection(int portno, int *sockfd)
{
   struct sockaddr_in serv_addr;
   int newsockfd;
   int fcntl_val;

   /* sockfd is the socket used to listen for connections
    * If that socket hasn't been created and bound to the
    * specified port number, do so here */
   if (*sockfd < 0)
   {
      /* Create a TCP/IP socket */
      *sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (*sockfd < 0)
      {
         perror ("socket");
         return -1;
      }
      /* Set the socket up to listen for INET (tcp/ip) connections
       * from any remote address using the specified port */
      memset(&serv_addr, 0, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port = htons(portno);
      if (bind(*sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      {
         perror ("bind");
         return -1;
      }
   }
   cerr << "Waiting for connection on port " << portno << endl;

   /* Listen for a connection. When one arrives, accept it.
    * Accepting creates the actual port for communication
    * for the rest of the program - save the port descriptor
    * as newsockfd.  */
   /* Note that sockfd will still be open to accept connections
    * the next time through this function, so don't reinit it each
    * time */
   if (listen(*sockfd,5) < 0)
   {
      perror ("listen");
      return -1;
   }
   newsockfd = accept(*sockfd, NULL, NULL);
   if (newsockfd < 0)
   {
      perror ("accept");
      return -1;
   }

   /* This might not be needed later.
    * Set the port up for non-blocking IO.  Non-blocking
    * means that reads from the port will always return immediately
    * even if there's no data. We'll use other tricks to see
    * if there is data available before calling read, and those tricks
    * will allow us to add a timeout. That timeout will help detect
    * cases where the client has disconnected without telling the
    * server, allowing the server to reset itself and handle those
    * types of errors */
   fcntl_val = fcntl(newsockfd, F_GETFL);
   if (fcntl(newsockfd, F_SETFL, fcntl_val | O_NONBLOCK) < 0)
   {
      perror ("fcntl for non-blocking");
      close (newsockfd);
      return -1;
   }

   return newsockfd;
}

/* Simple code to fill 4 bytes in a char * buffer from an int */
void fill_buffer(unsigned char *buf, unsigned int val)
{
   buf[0] = val >> 24;
   buf[1] = val >> 16;
   buf[2] = val >>  8;
   buf[3] = val      ;
}

int main(int argc, char *argv[])
{
     int sockfd = -1, newsockfd;
     unsigned char buffer[16];
     unsigned int value = 1;
     ssize_t buf_idx;
     ssize_t rc;
     int stop_server = 0;
     int close_connection = 0;

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

     if (argc < 2)
     {
	 cerr << "ERROR, need to provide a port number on the command line" << endl;
         exit(1);
     }

     /* TODO = add a command to shut down the server compeletly */
     /* Maybe add another to restart the server from scratch */
     while (!stop_server)
     {
           newsockfd = make_connection(atoi(argv[1]), &sockfd);
           if (newsockfd < 0)
           {
              /* This should never happen (famous last words) */
              perror("make_connection failed");
              continue;
           }
	   cerr << "Connected" << endl;
           /* Loop reading and responding to commands from the client */
           for (close_connection = 0; !close_connection; )
           {
              /* Loop to read 4 bytes. A read should never break up such
               * a small read, but better safe than sorry */
              for (buf_idx = 0; buf_idx < 4; )
              {
                 /* Ugly code to wait for data on newsockfd for
                  * timeout seconds.
                  * The FD_ZERO/FD_SET is making a list of sockets
                  * to wait for. Here we're only waiting for the one,
                  * but select() can be programmed to wait for data
                  * on multiple sockets - the code is a bit overly-
                  * complex for simple cases like this */
                 fd_set input;
                 FD_ZERO(&input);
                 FD_SET (newsockfd, &input);
                 /* Set the timeout for 10 seconds */
                 struct timeval timeout;
                 timeout.tv_sec  = 10;
                 timeout.tv_usec = 0;
                 /* select() waits for the socket fd in the input list
                  * to have data show up.  It will also return if no
                  * data is seen for timeout seconds.  The latter case
                  * most likely means the client has died so close
                  * the current connection and wait for the next one */
                 rc = select(newsockfd + 1, &input, NULL, NULL, &timeout);
                 if (rc <= 0)
                 {
                    /* 0 == timeout, -1 is an error. In either case,
                     * assume timeout or remote end closed connection */
                    if (rc < 0)
                       perror("select");
                    close_connection = 1;
                    break;
                 }

                 rc = read(newsockfd, &buffer[buf_idx], sizeof(buffer) - buf_idx - 1);
                 if (rc <= 0)
                 {
                    /* Same as above - assume errors mean timeout or
                     * remote end closed connection unexpectedly */
                    if (rc < 0)
                       perror("read");
                    close_connection = 1;
                    break;
                 }
                 /* if rc > 0 , rc is the number of bytes read.
                  * Hopefully this will be 4 */
                 buf_idx += rc;
              }
              /* If something happend and the server timed out
               * or saw another error, process that immediately
               * since there's nothing valid in buffer[] */
              if (close_connection)
              {
                cerr << "Closing connection due to error or timeout waiting for data" << endl;
              }
              else
              {
                 cerr << "Received " << hex << (unsigned int)buffer[0] << " " << (unsigned int)buffer[1] << " " << (unsigned int)buffer[2] << " " << (unsigned int)buffer[3] << endl;
                 if (buffer[3] == 0)
                 {
                    /* Orderly disconnect. Close the connection then loop back and
                     * start waiting for the next one */
                    close_connection = 1;
                    cerr << "Closing connection due to request from client" << endl;
                 }
                 else if (buffer[3] == 1)
                 {
                    /* Fill in dummy data.  This will be replaced by
                     * actual processed image (or whatever) data from the
                     * Jetson board.
                     * Initially, probably do an image capture/process
                     * each time the client requests it.
                     * For better performance, kick off a thread when
                     * the client connects that is constantly reading/processing
                     * When the client requests data return the most recent
                     * completely processed data from that separate thread.
                     * This will be similar to a typical multi-threaded
                     * consumer/producer problem, but where the consumer/
                     * producer pattern has the producer wait if the consumer isn't
                     * ready here we want to just overwrite old data and
                     * constantly replace it with newer values.
                     */
		    vector<Vec3f> xAndY;
		    displayCircles(inputVideo, xAndY);
                    fill_buffer(buffer  , xAndY[0][0]);
                    fill_buffer(buffer+4, xAndY[0][1]);
                    fill_buffer(buffer+8, xAndY[0][2]);
                    rc = write(newsockfd, buffer, 12);
                    if (rc < 0)
                    {
                       /* Assume timeout or remote end closed connection */
                       perror("ERROR writing to socket");
                       close_connection = 1;
                    }
                 }
              }
           }
           /* Close the connection. Note this only closes the one
            * created by accept().  The original socket is still
            * listening for new connections to pass off to listen/accept
            * next time around */
           close(newsockfd);
     }
     close(sockfd);
     return 0;
}



