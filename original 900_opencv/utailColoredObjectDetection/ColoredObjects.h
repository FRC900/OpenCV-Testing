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




#ifndef COLOREDOBJECTS_H_
#define COLOREDOBJECTS_H_

#include "CoeStructures.h"

namespace utail_coloredobjectdetection {

class ColoredObjects {
public:
	ColoredObjects();
	virtual ~ColoredObjects();
	void setParameters(int minArea,int maxSize);
	ColoredObject * getColoredObjects();
	ColoredObject * getBiggest();
	int getSize();
	bool add(float area, float pitch, float yaw, int x, int y);
	ColoredObject * coloredObjects;
	void reset();
private:
	int counter;
	int minArea,maxSize;
};

} /* namespace utail_coloredobjectdetection */
#endif /* COLOREDOBJECTS_H_ */
