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



#include "ContourFinder.h"

using namespace cv;


namespace utail_coloredobjectdetection {


ContourFinder::ContourFinder(Configuration * config) {
	storage = cvCreateMemStorage(0);
	this->config = config;
	coloredObjects = new ColoredObjects[config->NBCOLORS];
	for(int i=0;i<config->NBCOLORS;i++){
		coloredObjects[i].setParameters(config->minArea,config->maxSize);
	}
}

ContourFinder::~ContourFinder() {
	delete[] coloredObjects;
}

float inline ContourFinder::convertCoordToAngle(bool xCoord,int coord, int screenSize) {
	if (xCoord){
		return  -(((float)coord)*config->xFOV/screenSize)*0.01745329;
	} else {
		return  -(((float)coord)*config->yFOV/screenSize)*0.01745329;
	}
}

void ContourFinder::detectContours(int color,IplImage *raw,IplImage *filtered){
    cvClearMemStorage(storage);
    coloredObjects[color].reset();
	cvInRangeS(raw,config->minMaxColors[color].min,config->minMaxColors[color].max,filtered);
	cvFindContours(filtered, storage, &contours,sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	added = true;
	for(;contours;contours = contours->h_next){
	  if (!added) break;
	  area = cvContourArea( contours , CV_WHOLE_SEQ );
	  if (area>config->minArea){
		  rect = cvBoundingRect(contours,0);
		  p1.x = rect.x;p1.y = rect.y;p2.x = rect.x+rect.width;p2.y = rect.y+rect.height;
		  center.x = (p1.x+p2.x)/2;center.y = (p1.y+p2.y)/2;
		  coloredObject.area = area;
		  coloredObject.x = center.x;
		  coloredObject.y = center.y;
		  coloredObject.pitch = convertCoordToAngle(false,center.y-cvRound(raw->height/2),raw->height);
		  coloredObject.yaw = convertCoordToAngle(true,center.x-cvRound(raw->width/2),raw->width);
		  coloredObject.x = cvRound( coloredObject.x - (raw->width/2) );
		  coloredObject.y = cvRound( coloredObject.y - (raw->height/2) );
          added = coloredObjects[color].add(coloredObject.area,coloredObject.pitch,coloredObject.yaw,coloredObject.x,coloredObject.y);
          if (config->isUsingGUI()){
              cvRectangle(raw,p1,p2,config->drawingColors[color],5);
    		  cvCircle(raw,center,1,config->drawingColors[color],5);
    		  cvRectangle(filtered,p1,p2,config->drawingColors[color],5);
    		  cvCircle(filtered,center,1,config->drawingColors[color],5);
          }
	  }
	}
}

int ContourFinder::getSize(int color){
	return coloredObjects[color].getSize();
}

ColoredObject *  ContourFinder::getColoredObjects(int color){
	return coloredObjects[color].getColoredObjects();
}

ColoredObject *  ContourFinder::getBiggest(int color){
	return coloredObjects[color].getBiggest();
}


} /* namespace utail_coloredobjectdetection */
