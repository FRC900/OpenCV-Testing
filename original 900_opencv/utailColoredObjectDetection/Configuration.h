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



#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include "CoeStructures.h"

namespace utail_coloredobjectdetection {

class Configuration {
public:
	Configuration(bool useGui);
	virtual ~Configuration();
	bool isUsingGUI();
	enum Colors {
		RED, BLUE, GREEN, YELLOW, NBCOLORS
	};
	static const int maxSize = 20; // will store in memory only the first maxSize detected objects (per color)
	static const int minArea = 200; // in pixels. Objects of lower size will be ignored
	static const float xFOV = 60.87; // field of vision, in degree
	static const float yFOV = 47.64; // field of vision, in degree
	void setDetect(int color,bool detect);
	bool isDetecting(int color);
	MinMaxColors * minMaxColors;
	CvScalar * drawingColors;
	std::string * printColors;
	bool * colorDetect;
private:
	bool useGui; // display windows (or not)
};

} /* namespace utail_coloredobjectdetection */
#endif /* CONFIGURATION_H_ */
