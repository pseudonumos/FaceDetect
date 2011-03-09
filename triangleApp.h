#ifndef _TRIANGLE_APP

#include <stdio.h>
#include "glfw.h"
#include "simpleApp.h"
#include "imageTexture.h"
#include "videoInput.h"

// Include header files
#include "cv.h"
#include "highgui.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <sstream>
//for threading
#include <process.h>


class triangleApp : public simpleApp{
	
	public:
		triangleApp();
		virtual void idle();
		virtual void init();
		virtual void draw();
		virtual void triangleApp::snapshot();
		virtual void triangleApp::findFace();
		virtual void triangleApp::getBackground();
		virtual void keyDown  (char c);
		virtual void triangleApp::saveImage();
		void triangleApp::VItoIPL(unsigned char * src, IplImage * dst);

		static void triangleApp::triangulate(int x1, int y1, int x2, int y2, CvMat R, CvMat T);
		static void triangleApp::putEverythingTogether();
		static void triangleApp::printCoordinates(CvScalar color, CvPoint coord, CvScalar color2, CvPoint coord2);
		
		unsigned char * s;
		unsigned char * s2;
		unsigned char * s3;


		unsigned char * b1;
		unsigned char * b2;
		unsigned char * b3;

private:
		static void __cdecl procThread(void * objPtr);
		static void __cdecl procThread2(void * objPtr);
		static void __cdecl procThread3(void * objPtr);
		static void __cdecl triangleApp::frontThread(void * objPtr);
		static void __cdecl triangleApp::frontThread2(void * objPtr);
		static void __cdecl triangleApp::frontThread3(void * objPtr);
		static void __cdecl triangleApp::profileThread(void * objPtr);
		static void __cdecl triangleApp::profileThread2(void * objPtr);
		static void __cdecl triangleApp::profileThread3(void * objPtr);
		static void __cdecl triangleApp::drawRectangles(void * objPtr);
		static void __cdecl triangleApp::drawRectangles2(void * objPtr);
		static void __cdecl triangleApp::drawRectangles3(void * objPtr);

};

#endif	// _triangle_APP