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




#include "ColoredObjectExtractor.h"

namespace utail_coloredobjectdetection {

ColoredObjectExtractor::ColoredObjectExtractor(Configuration * config) {
	this->config = config;
	imageFiltered = 0;
	if (config->isUsingGUI()){
		imageY = 0;
		imageU = 0;
		imageV = 0;
	}
	contourFinder = new ContourFinder(config);
}

ColoredObjectExtractor::~ColoredObjectExtractor() {
	delete contourFinder;
	if (imageFiltered){
		cvReleaseImageHeader( &imageFiltered );
		if (config->isUsingGUI()){
			cvReleaseImageHeader( &imageY );
			cvReleaseImageHeader( &imageU );
			cvReleaseImageHeader( &imageV );
			cvDestroyWindow( imageRawName.c_str() );
			cvDestroyWindow( imageFilteredName.c_str() );
			cvDestroyWindow( imageYName.c_str() );
			cvDestroyWindow( imageUName.c_str() );
			cvDestroyWindow( imageVName.c_str() );
		}
	}
	delete config;
}

ColoredObject * ColoredObjectExtractor::getColoredObjects(int color){
	return contourFinder->getColoredObjects(color);
}

ColoredObject * ColoredObjectExtractor::getBiggest(int color){
	return contourFinder->getBiggest(color);
}

int ColoredObjectExtractor::getSize(int color){
	return contourFinder->getSize(color);
}

void ColoredObjectExtractor::detect(){
	for(int color=0;color<config->NBCOLORS;color++){
		if (config->colorDetect[color]){
			contourFinder->detectContours(color,imageRaw,imageFiltered);
		}
	}
	if (config->isUsingGUI()){
		cvShowImage(imageRawName.c_str(), imageRaw);
		cvShowImage(imageYName.c_str(), imageY);
		cvShowImage(imageUName.c_str(), imageU);
		cvShowImage(imageVName.c_str(), imageV);
		cvShowImage(imageFilteredName.c_str(), imageFiltered);
	}
}

void ColoredObjectExtractor::setImage(IplImage *image){ // Note : YUV image expected
	imageRaw = image;
	if (!imageFiltered){
		imageFiltered = cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U, 1);
		if (config->isUsingGUI()){
			imageRawName = "Raw Image";
			imageFilteredName = "Filtered image";
			imageY = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			imageYName = "Y component";
			imageU = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			imageUName = "U component";
			imageV = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			imageVName = "V component";
			cvNamedWindow(imageRawName.c_str(), CV_WINDOW_AUTOSIZE);
			cvNamedWindow(imageFilteredName.c_str(), CV_WINDOW_AUTOSIZE);
			cvNamedWindow(imageUName.c_str(), CV_WINDOW_AUTOSIZE);
			cvNamedWindow(imageVName.c_str(), CV_WINDOW_AUTOSIZE);
			cvNamedWindow(imageYName.c_str(), CV_WINDOW_AUTOSIZE);
		}
	}
	if (config->isUsingGUI()){
		cvSplit(imageRaw,imageY,imageU,imageV,0);
	}
}


}
