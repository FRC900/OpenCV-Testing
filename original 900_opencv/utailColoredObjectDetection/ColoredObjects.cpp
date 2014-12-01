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


#include "ColoredObjects.h"

namespace utail_coloredobjectdetection {

ColoredObjects::ColoredObjects() {
	counter = 0;
}

ColoredObjects::~ColoredObjects() {
	delete[] coloredObjects;
}

void ColoredObjects::setParameters(int minArea,int maxSize){
	this->minArea = minArea;
	this->maxSize = maxSize;
	coloredObjects = new ColoredObject[maxSize];
}

void ColoredObjects::reset(){
	counter = 0;
}

bool ColoredObjects::add(float area, float pitch, float yaw, int x, int y){
	if (counter>maxSize) return false;
	if (area<=minArea) return false;
	coloredObjects[counter].area = area;
	coloredObjects[counter].pitch = pitch;
	coloredObjects[counter].yaw = yaw;
	coloredObjects[counter].x = x;
	coloredObjects[counter].y = y;
	counter++;
	return true;
}

ColoredObject * ColoredObjects::getColoredObjects(){
	return coloredObjects;
}

int ColoredObjects::getSize(){
	return counter;
}

ColoredObject * ColoredObjects::getBiggest(){
	int maxArea = 0;
	ColoredObject* maxObject = 0;
	for(int i=0;i<counter;i++){
		if (coloredObjects[i].area>maxArea){
			maxArea = coloredObjects[i].area;
			maxObject = &coloredObjects[i];
		}
	}
	return maxObject;
}

} /* namespace utail_coloredobjectdetection */
