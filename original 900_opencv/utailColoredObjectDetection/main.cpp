/*

	UTAIL Colored Object Detection. Copyright 2012 Vincent Berenz and The University of Tsukuba Artificial Intelligence Laboratory.

	This file is part of UTAIL Colored Object Detection.

    UTAIL Colored Object Detection is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    UTAIL Colored Object Detection is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with UTAIL Colored Object Detection.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "math.h"
#include <iostream>
#include "ColoredObjectExtractor.h"
#include "Configuration.h"

using namespace std;
using namespace cv;
using namespace utail_coloredobjectdetection;


int main( int argc, const char* argv[] ) {

	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);

	IplImage *img = 0;
	IplImage *yuv = 0;

	Configuration * config = new Configuration(true);
	config->setDetect(config->RED,true);
	config->setDetect(config->BLUE,true);
	ColoredObjectExtractor * coe = new ColoredObjectExtractor(config);

	ColoredObject *coloredObjects,*biggestObject;
	int color,size,i;

	while ( 1 ) {

		img = cvQueryFrame( capture );
		if (yuv==0) yuv=cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 3);
		cvCvtColor(img,yuv,CV_RGB2YCrCb);

		coe->setImage(yuv);
		coe->detect();

		cout << "\n\n---------------------------------------------\n";
		for(color=0;color<config->NBCOLORS;color++){
			if (config->isDetecting(color)){
				size = coe->getSize(color);
				cout << "detecting " << size << " " << config->printColors[color] << " objects\n";
				if (size>0){
					biggestObject = coe->getBiggest(color);
					coloredObjects = coe->getColoredObjects(color);
					for (i=0;i<size;i++){
						if (&coloredObjects[i] == biggestObject) cout<<"(biggest) ";
						cout << "area\t"<<coloredObjects[i].area<<"\tx\t"<<coloredObjects[i].x<<"\ty\t"<<coloredObjects[i].y<<"\n";
					}
				}
			}
		}
		cout << "---------------------------------------------\n";

		if ( (cvWaitKey(10) & 255) == 27 ) break;
	}

	cvReleaseImage(&yuv);
	cvReleaseImage(&img);
	delete coe;
	delete config;

}
