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



#ifndef COLOREDOBJECTEXTRACTOR_H_
#define COLOREDOBJECTEXTRACTOR_H_

#include "CoeStructures.h"
#include "ContourFinder.h"
#include "Configuration.h"
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "math.h"

using namespace std;
using namespace cv;

namespace utail_coloredobjectdetection {

class ColoredObjectExtractor {
public:
	ColoredObjectExtractor(Configuration * config);
	virtual ~ColoredObjectExtractor();
	void setImage(IplImage *image);
	void detect();
	ColoredObject * getColoredObjects(int color);
	ColoredObject * getBiggest(int color);
	int getSize(int color);
private:
	Configuration * config;
	ContourFinder * contourFinder;
	IplImage *imageRaw, *imageFiltered;
	IplImage *imageY,*imageU,*imageV;
	std::string imageFilteredName,imageRawName,imageYName,imageUName,imageVName;
};





} /* namespace utail_coloredobjectdetection */
#endif /* COLOREDOBJECTEXTRACTOR_H_ */
