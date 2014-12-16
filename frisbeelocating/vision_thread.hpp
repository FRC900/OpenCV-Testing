#ifndef VISION_THREAD_H__
#define VISION_THREAD_H__

// add typedef or class for data returned to server code. Make 
// visionThread fill this in so that the server can just blindly 
// return the buffer to the client. This makes less of the code 
// need to know the details of what's being processed - the server
// code shouldn't have to be changed each time the details of
// vision processing changes

#include <vector>
typedef std::vector<unsigned char> VisionBuf;

void *visionThread(void *data);
#endif

