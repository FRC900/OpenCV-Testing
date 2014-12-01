

UTAIL Colored Object Detection is an open source C++ OpenCV based computer vision program.
It has been tested exclusively with OpenCV version 2.1.0.
It has been developped by Vincent Berenz at the University of Tsukuba Artificial Intelligence Laboratory.
Vincent Berenz website: http://www.ai.iit.tsukuba.ac.jp/~vincent/
University of Tsukuba Artificial Intelligence Laboratory website: http://ai.iit.tsukuba.ac.jp

For more information about this module, visit:
http://www.ai.iit.tsukuba.ac.jp/~vincent/opensource.html

main.cpp gives an example of usage of UTAIL Colored Object Detection. 

By default, green, blue, red and yellow objects are detected. To extend this list of colors:

- Extends the enumeration at line 40 of Configuration.cpp
- Update functions getMinMaxColors, getDrawingColors and getPrintColors in Configuration.cpp

Note: A module encapsulating UTAIL Colored Object Detection targeting usage on Aldebaran's humanoid robot Nao can be found:
http://www.ai.iit.tsukuba.ac.jp/~vincent/tdm/

