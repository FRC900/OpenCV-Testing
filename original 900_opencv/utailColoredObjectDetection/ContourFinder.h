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


#ifndef CONTOURFINDER_H_
#define CONTOURFINDER_H_

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "ColoredObjects.h"
#include "Configuration.h"

namespace utail_coloredobjectdetection {

class ContourFinder {
public:
	ContourFinder(Configuration * config);
	virtual ~ContourFinder();
	void detectContours(int color,IplImage *raw,IplImage *filtered);
	int getSize(int color);
	ColoredObject * getColoredObjects(int color);
	ColoredObject * getBiggest(int color);
private:
	float inline convertCoordToAngle(bool xCoord,int coord, int screenSize);
	Configuration * config;
	ColoredObjects * coloredObjects;
	ColoredObject coloredObject;
	double area;
	CvSeq *contours;
	CvRect rect;
	CvPoint center,p1,p2;
	CvMemStorage* storage;
	bool added;
};


}
#endif
