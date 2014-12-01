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



#include "Configuration.h"
#include "CoeStructures.h"

namespace utail_coloredobjectdetection {

MinMaxColors * getMinMaxColors(){
	// min and max values in YUV space
	MinMaxColors * minMaxColors = new MinMaxColors[Configuration::NBCOLORS];
	minMaxColors[Configuration::RED].min = CV_RGB(200,0,0);
	minMaxColors[Configuration::RED].max = CV_RGB(255,150,150);
	minMaxColors[Configuration::BLUE].min = CV_RGB(0,150,50);
	minMaxColors[Configuration::BLUE].max = CV_RGB(100,255,200);
	minMaxColors[Configuration::GREEN].min = CV_RGB(0,50,200);
	minMaxColors[Configuration::GREEN].max = CV_RGB(90,150,255);
	minMaxColors[Configuration::YELLOW].min = CV_RGB(50,0,200);
	minMaxColors[Configuration::YELLOW].max = CV_RGB(200,100,250);
	return minMaxColors;
}

CvScalar * getDrawingColors(){
	CvScalar *  colors = new CvScalar[Configuration::NBCOLORS];
	colors[Configuration::RED] = CV_RGB(255,0,0);
	colors[Configuration::GREEN] = CV_RGB(0,255,0);
	colors[Configuration::BLUE] = CV_RGB(0,0,255);
	colors[Configuration::YELLOW] = CV_RGB(255,255,50);
	return colors;
}

std::string * getPrintColors(){
	std::string *colors = new std::string[Configuration::NBCOLORS];
	colors[Configuration::RED] = "RED";
	colors[Configuration::BLUE] = "BLUE";
	colors[Configuration::GREEN] = "GREEN";
	colors[Configuration::YELLOW] = "YELLOW";
	return colors;
}


Configuration::Configuration(bool useGui){
	this->useGui = useGui;
	minMaxColors = getMinMaxColors();
	drawingColors = getDrawingColors();
	printColors = getPrintColors();
	colorDetect = new bool[NBCOLORS];
	for (int i=0;i<NBCOLORS;i++) colorDetect[i] = false;
}

Configuration::~Configuration() {
	delete[] minMaxColors;
	delete[] drawingColors;
	delete[] printColors;
	delete[] colorDetect;
}

bool Configuration::isUsingGUI(){
	return useGui;
}

void Configuration::setDetect(int color,bool detect){
	try {
		colorDetect[color] = detect;
	} catch (int e) {
		//
	}
}

bool Configuration::isDetecting(int color){
	try {
		return colorDetect[color];
	} catch(int e){
		return false;
	}
	return false;
}

} /* namespace utail_coloredobjectdetection */
