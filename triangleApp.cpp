#include "triangleApp.h"

//Initialize Devices
static int dev = 0, dev2 = 1, dev3 = 2;
static int camSize1 = 0;
static int camSize2 = 0;
static int camSize3 = 0;
static int cameraWidth = 0;
static int cameraHeight = 0;

//Background Subtraction
static int count = 0;
static bool subtract = false;
static bool getBack = false;
static bool maskInit = false;

// For Threading
static videoInput     VI;
static imageTexture * IT;
static imageTexture * IT2;
static imageTexture * IT3;
static imageTexture * i1;
static imageTexture * i2;
static imageTexture * i3;

static unsigned char * img1;
static unsigned char * img2;
static unsigned char * img3;
static unsigned char * img4;
static unsigned char * img5;
static unsigned char * img6;
static unsigned char * c1;
static unsigned char * c2;
static unsigned char * c3;

static unsigned char * f;
static unsigned char * f2;
static unsigned char * f3;

static unsigned char * frame;
static unsigned char * frame2;
static unsigned char * frame3;

static unsigned char * map;
static imageTexture * mapDisplay;

//Threading variables
static bool facedone1 = true;
static bool facedone2 = true;
static bool facedone3 = true;
static bool facedone4 = true;
static bool facedone5 = true;
static bool facedone6 = true;
static bool facegrabbed1 = false;
static bool facegrabbed2 = false;
static bool facegrabbed3 = false;
static bool facesdrawn1 = true;
static bool facesdrawn2 = true;
static bool facesdrawn3 = true;
static bool facesdrawn4 = true;
static bool facesdrawn5 = true;
static bool facesdrawn6 = true;
static bool facedisplayed1 = true;
static bool facedisplayed2 = true;
static bool facedisplayed3 = true;

//Switching variables
static bool useProfile = true;
static bool useFrontface = true;
static bool useSkinFilter = true;
static bool motionsubtract = true;
static bool motionfilter = false;
static bool chooseDetection = true;
static bool useThreads = true;

//Face drawing variables

static CvSeq * frontfaces1 = 0;
static CvSeq * frontfaces2 = 0;
static CvSeq * frontfaces3 = 0;
static CvSeq * profilefaces1 = 0;
static CvSeq * profilefaces2 = 0;
static CvSeq * profilefaces3 = 0;

int frontalFaceCount1 = 0;
int frontalFaceCount2 = 0;
int frontalFaceCount3 = 0;

//Framing the right image variables
static CvPoint topLeft = {2,2};
static CvPoint bottomRight = {637,477};
static CvScalar frameColor = CV_RGB(0, 0, 255);


//Orientation variables
static CvPoint xLength = {0, 0};
static CvPoint xLength2 = {0, 0};
static CvPoint xLength3 = {0, 0};

//Center of Mass calculation
static double alpha = .8, beta = .8, gamma = .8;
static bool alphaInit = false, betaInit = false, gammaInit = false;

//Triangulation Point
static CvPoint com1 = {0, 0}, com2 = {0, 0}, com3 = {0, 0}, trianglePt = {0, 0}, trianglePt2 = {0, 0};
static int count1 = 0, count2 = 0, count3;

static double rot21[] = {0.4449, -0.0726, 0.8926, 0.0524, 0.9971, 0.0550, -0.8940, 0.0223, 0.4474};
//{-0.2064, -0.0116, 0.9784, -0.0874, 0.9961, -0.0067, -0.9746, -0.0869, -0.2066};
static double rot32[] = {-0.1416, -0.0897, 0.9859, -0.0087, 0.9960, 0.0893, -0.9899, 0.0041, -0.1418};
//{ 0.0921, -0.0644, 0.9937, -0.0228, 0.9975, 0.0668, -0.9955, -0.0288, 0.0904};
static double rot31[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static double trans21[] = {-2166.62045, -121.30195, 1125.95653};
//{-55.99728, 7.37738, 74.33608};
static double trans32[] = {-2463.68140, -108.67394, 2702.04887};
//{-83.61882, -0.66442, 74.18159};
static double trans31[] = {0, 0, 0};
static double focal = 535.0;
static double xpos = 0;
static double zpos = 0;

static CvMat R21 = cvMat(3, 3, CV_64FC1, rot21);
static CvMat R32 = cvMat(3, 3, CV_64FC1, rot32);
static CvMat R31 = cvMat(3, 3, CV_64FC1, rot31);
static CvMat T21 = cvMat(3, 1, CV_64FC1, trans21);
static CvMat T32 = cvMat(3, 1, CV_64FC1, trans32);
static CvMat T31 = cvMat(3, 1, CV_64FC1, trans31);

//Camera positions
static CvPoint cam1 = {0, 0};
static CvPoint cam2 = {0, 0};
static CvPoint cam3 = {0, 0};
static CvPoint camDirection1 = {0,0};
static CvPoint camDirection2 = {0,0};
static CvPoint camDirection3 = {0,0};
static CvScalar cameraColor = CV_RGB(0, 255, 0);

//Map Window Size
static int mapWidth = 640;
static int mapHeight = 480;
static double mapScale = 10;
static CvPoint mapCenter = {mapWidth/2, mapHeight/2};

//Skin Filter Matrix
//Skin filter for face detection
double meanr = 95;
double meanb = 150;

double coa = .3386;
double cob = .0408;
double coc = .0408;
double cod = .2493;

// Create a string that contains the exact cascade name
//const char* cascade_name = "C:/Program Files/OpenCV/data/haarcascades/haarcascade_frontalface_alt.xml";
const char* cascade_name = "C:/Program Files/OpenCV/data/haarcascades/haarcascade_frontalface_alt2.xml";
const char* cascade_name2 =	 "C:/Program Files/OpenCV/data/haarcascades/haarcascade_frontalface_default.xml";
const char* cascade_name3 = "C:/Program Files/OpenCV/data/haarcascades/haarcascade_profileface.xml";

/*    "haarcascade_profileface.xml";*/

//empty constructor
triangleApp::triangleApp(){
	
}

void triangleApp::init(){

	//optional static function to list devices
	//for silent listDevices use listDevices(true);
	int numDevices = videoInput::listDevices();	

	//uncomment for silent setup
	//videoInput::setVerbose(false);

	//we allocate our openGL texture objects
	//we give them a max size of 1024 by 1024 pixels
	IT  = new imageTexture(640,480, GL_RGB);    	
	IT2 = new imageTexture(640,480, GL_RGB);
	IT3 = new imageTexture(640,480, GL_RGB);

	i1 = new imageTexture(640, 480, GL_RGB);
	i2 = new imageTexture(640, 480, GL_RGB);
	i3 = new imageTexture(640, 480, GL_RGB);

	mapDisplay = new imageTexture(mapWidth, mapHeight, GL_RGB);

	//by default we use a callback method
	//this updates whenever a new frame
	//arrives if you are only ocassionally grabbing frames
	//you might want to set this to false as the callback caches the last
	//frame for performance reasons. 
	VI.setUseCallback(true);

	//try and setup device with id 0 and id 1
	//if only one device is found the second 
	//setupDevice should return false

	//we can specifiy the dimensions we want to capture at
	//if those sizes are not possible VI will look for the next nearest matching size
	VI.setupDevice(dev,   640, 480, VI_COMPOSITE); 
	VI.setupDevice(dev+1, 640, 480, VI_COMPOSITE);
	VI.setupDevice(dev+2, 640, 480, VI_COMPOSITE);

	camSize1 = VI.getSize(dev);
	camSize2 = VI.getSize(dev+1);
	camSize3 = VI.getSize(dev+2);
	cameraWidth = VI.getWidth(dev);
	cameraHeight = VI.getHeight(dev);
	
	//once the device is setup you can try and
	//set the format - this is useful if your device
	//doesn't remember what format you set it to
	//VI.setFormat(dev, VI_NTSC_M);					//optional set the format

	//we allocate our buffer based on the number
	//of pixels in each frame - this will be width * height * 3
	frame = new unsigned char[camSize1];
	frame2 = new unsigned char[camSize2];
	frame3 = new unsigned char[camSize3];
	f = new unsigned char[camSize1];
	f2 = new unsigned char[camSize2];
	f3 = new unsigned char[camSize3];
	s = new unsigned char[camSize1];
	s2 = new unsigned char[camSize2];
	s3 = new unsigned char[camSize3];

	img1 = new unsigned char[camSize1];
	img2 = new unsigned char[camSize2];
	img3 = new unsigned char[camSize3];
	img4 = new unsigned char[camSize1];
	img5 = new unsigned char[camSize2];
	img6 = new unsigned char[camSize3];
	c1 = new unsigned char[camSize1];
	c2 = new unsigned char[camSize2];
	c3 = new unsigned char[camSize3];

	b1 = new unsigned char[camSize1];
	b2 = new unsigned char[camSize2];
	b3 = new unsigned char[camSize3];

	map = new unsigned char[mapWidth*mapHeight*3];

	cvMatMul(&R21, &R32, &R31);
	cvMatMul(&R21, &T32, &T31);
	cvAdd(&T21, &T31, &T31);
	cam1.x = 0;
	cam1.y = cvRound(mapCenter.y);
	cam2.x = cvRound(trans21[2]/mapScale);
	cam2.y = cvRound(-trans21[0]/mapScale) + mapCenter.y;
	cam3.x = cvRound(trans31[2]/mapScale);
	cam3.y = cvRound(-trans31[0]/mapScale) + mapCenter.y;
	
	double direc[] = {0,0,20};
	CvMat newDirection = cvMat(3, 1, CV_64FC1, direc);
	camDirection1.x = 20;
	camDirection1.y = cvRound(mapCenter.y);
	cvMatMul(&R21, &newDirection, &newDirection);
	camDirection2.x = cam2.x + cvRound(cvmGet(&newDirection,2,0));
	camDirection2.y = cam2.y + cvRound(- cvmGet(&newDirection,0,0));
	double direc2[] = {0, 0, 20};
	newDirection = cvMat(3, 1, CV_64FC1, direc2);
	cvMatMul(&R31, &newDirection, &newDirection);
	camDirection3.x = cam3.x + cvRound(cvmGet(&newDirection,2,0));
	camDirection3.y = cam3.y + cvRound(- cvmGet(&newDirection,0,0));

}

void __cdecl triangleApp::procThread(void * objPtr) {

//	videoDevice * vd = *( (videoDevice **)(objPtr) )
	int * deviceId  = ((int *) (objPtr));

	IplImage * temp1 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * temp2 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * diff1 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * g1 = cvCreateImage(cvSize(640, 480), 8, 1);
	
	//need temp variables as we are converting from a char* to an IplImage* as we are using
	//two different libraries
	temp1->imageData = (char *) f;
	temp2->imageData = (char *) frame;
	//Differencing algorithm	
	cvAbsDiff(temp1, temp2, diff1);
	//convert from RGB to grayscale
	cvCvtColor(diff1, g1, CV_RGB2GRAY);
	//perform edge detection
	cvCanny(g1, g1, 20, 55);

	cvSmooth( g1, g1, CV_GAUSSIAN, 9, 9 ); // smooth it, otherwise a lot of false circles may be detected
	cvThreshold(g1, g1, 50, 255, CV_THRESH_BINARY);

	CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour = 0;
	//find contours in altered image g1, store them in the cvSeq * contour)
    cvFindContours( g1, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	CvPoint center = {0, 0};
	CvPoint * temp;
	CvSeq* biggestContour = 0;
	int maxTotal = 0;
    CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
    for( ; contour != 0; contour = contour->h_next ) {
		CvSeq * result = contour;
		int tot = result->total;
		if (tot < 100) continue;
		if (tot <= maxTotal) continue;
		maxTotal = tot;
		biggestContour = result;
	}
	for (int i = 0; i < maxTotal; i++) {
		temp = (CvPoint*) cvGetSeqElem(biggestContour, i);
		center.x += temp->x;
		center.y += temp->y;
	}
	if (maxTotal != 0) {
		center.x = cvRound(center.x / maxTotal);
		center.y = center.y / maxTotal;
		//set flag variable to true
		if (!alphaInit) {
			com1.x = center.x;
			com1.y = center.y;
			alphaInit = true;
		}
		com1.x = cvRound(alpha*com1.x + (1-alpha)*center.x);
		com1.y = cvRound(alpha*com1.y + (1-alpha)*center.y);

		CvRect r = ((CvContour*)biggestContour)->rect;
		xLength.x = cvRound(r.x-r.width * 0.1);
		xLength.y = cvRound(r.x + r.width * 1.1);

		cvReleaseImage(&temp1);
		cvReleaseImage(&temp2);
		cvReleaseImage(&diff1);
		cvReleaseImage(&g1);
		if (alphaInit && betaInit && gammaInit) {
			putEverythingTogether();
		}
	}
	return;
}
//SAME CODE AS ABOVE ONLY FOR CAMERA #2
void __cdecl triangleApp::procThread2(void * objPtr) {

//	videoDevice * vd = *( (videoDevice **)(objPtr) )
	int * deviceId  = ((int *) (objPtr));

	IplImage * temp3 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * temp4 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * diff2 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * g2 = cvCreateImage(cvSize(640, 480), 8, 1);
	
	temp3->imageData = (char *) f2;
	temp4->imageData = (char *) frame2;
	
	cvAbsDiff(temp3, temp4, diff2);
	cvCvtColor(diff2, g2, CV_RGB2GRAY);
	cvCanny(g2, g2, 20, 55);
	cvSmooth( g2, g2, CV_GAUSSIAN, 9, 9 ); // smooth it, otherwise a lot of false circles may be detected
	cvThreshold(g2, g2, 50, 255, CV_THRESH_BINARY);

	CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour2 = 0;
    cvFindContours( g2, storage, &contour2, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	CvPoint center2 = {0, 0};
	CvPoint * tempt;
	CvSeq* biggestContour2 = 0;
	int maxTotal2 = 0;
    CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
    for( ; contour2 != 0; contour2 = contour2->h_next ) {
		CvSeq * result2 = contour2;
		int tot2 = result2->total;
		if (tot2 < 100) continue;
		if (tot2 <= maxTotal2) continue;
		maxTotal2 = tot2;
		biggestContour2 = contour2;
	}
	for (int j = 0; j < maxTotal2; j++) {
		tempt = (CvPoint*) cvGetSeqElem(biggestContour2, j);
		center2.x += tempt->x;
		center2.y += tempt->y;
	}
	if (maxTotal2 != 0) {
		center2.x = center2.x / maxTotal2;
		center2.y = center2.y / maxTotal2;
		if (!betaInit) {
			com2.x = center2.x;
			com2.y = center2.y;
			betaInit = true;
		}
		com2.x = cvRound(beta*com2.x + (1-beta)*center2.x);
		com2.y = cvRound(beta*com2.y + (1-beta)*center2.y);

		CvRect r2 = ((CvContour*)biggestContour2)->rect;
		xLength2.x = cvRound(r2.x-r2.width * 0.1);
		xLength2.y = cvRound(r2.x + r2.width * 1.1);

		cvReleaseImage(&temp3);
		cvReleaseImage(&temp4);
		cvReleaseImage(&diff2);
		cvReleaseImage(&g2);
		if (alphaInit && betaInit && gammaInit) {
			putEverythingTogether();
		}
	}
	return;
}
//SAME CODE AS procThread2 AND procThread1 JUST ADJUSTED FOR CAMERA #3
void __cdecl triangleApp::procThread3(void * objPtr) {

//	videoDevice * vd = *( (videoDevice **)(objPtr) )
	int * deviceId  = ((int *) (objPtr));

	IplImage * temp5 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * temp6 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * diff3 = cvCreateImage(cvSize(640, 480), 8, 3);
	IplImage * g3 = cvCreateImage(cvSize(640, 480), 8, 1);
	
	temp5->imageData = (char *) f3;
	temp6->imageData = (char *) frame3;
	
	cvAbsDiff(temp5, temp6, diff3);
	cvCvtColor(diff3, g3, CV_RGB2GRAY);
	cvCanny(g3, g3, 20, 55);
	cvSmooth( g3, g3, CV_GAUSSIAN, 9, 9 ); // smooth it, otherwise a lot of false circles may be detected
	cvThreshold(g3, g3, 50, 255, CV_THRESH_BINARY);

	CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* contour3 = 0;
    cvFindContours( g3, storage, &contour3, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );

	CvPoint center3 = {0, 0};
	CvPoint * tempto;
	CvSeq* biggestContour3 = 0;
	int maxTotal3 = 0;
    CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
    for( ; contour3 != 0; contour3 = contour3->h_next ) {
		CvSeq * result3 = contour3;
		int tot3 = result3->total;
		if (tot3 < 100) continue;
		if (tot3 <= maxTotal3) continue;
		maxTotal3 = tot3;
		biggestContour3 = contour3;
	}
	for (int j = 0; j < maxTotal3; j++) {
		tempto = (CvPoint*) cvGetSeqElem(biggestContour3, j);
		center3.x += tempto->x;
		center3.y += tempto->y;
	}
	if (maxTotal3 != 0) {
		center3.x = center3.x / maxTotal3;
		center3.y = center3.y / maxTotal3;
		if (!gammaInit) {
			com3.x = center3.x;
			com3.y = center3.y;
			gammaInit = true;
		}
		com3.x = cvRound(gamma*com3.x + (1-gamma)*center3.x);
		com3.y = cvRound(gamma*com3.y + (1-gamma)*center3.y);

		CvRect r3 = ((CvContour*)biggestContour3)->rect;
		xLength3.x = cvRound(r3.x-r3.width * 0.1);
		xLength3.y = cvRound(r3.x + r3.width * 1.1);

		cvReleaseImage(&temp5);
		cvReleaseImage(&temp6);
		cvReleaseImage(&diff3);
		cvReleaseImage(&g3);
		if (alphaInit && betaInit && gammaInit) {
			putEverythingTogether();
		}
	}
	return;
}




void triangleApp::idle(){
//is there a new frame?

	if (VI.isFrameNew(dev)) {
		count1++;
		//we alternate frames so we can take the difference between frames
		if (count1 % 2 == 0) {
			VI.getPixels(dev, f, true, false);
			IT->loadImageData(f, cameraWidth, cameraHeight, GL_RGB);
			if (!facegrabbed1 && chooseDetection) {
				facegrabbed1 = true;
				//because we are threading we don't want to pass the same copy around
				//so make multiple copies
				memcpy(img1, f, camSize1);
				memcpy(img4, f, camSize1);
				memcpy(c1, f, camSize1);
				//set threading variables
				facedone1 = false;
				facedone4 = false;
			}
		} else if (count1 % 2 == 1) {
			VI.getPixels(dev, frame, true, false);
			IT->loadImageData(frame, cameraWidth, cameraHeight, GL_RGB);
			if (!facegrabbed1 && chooseDetection) {
				facegrabbed1 = true;
				//make multiple copies for the threads
				memcpy(img1, frame, camSize1);
				memcpy(img4, frame, camSize1);
				memcpy(c1, frame, camSize1);
				//set threading variables
				facedone1 = false;
				facedone4 = false;
			}
		}
		if (useThreads) {
			HANDLE myThread;
			myThread = (HANDLE)_beginthread(procThread, 0, (void *) &dev);
		}
	}
	//repeate for camera2
	if( VI.isFrameNew(dev+1) )						
	{	
		count2++;
		//we alternate frames so we can take the difference between frames
		if (count2 % 2 == 0) {
			VI.getPixels(dev+1, f2, true, false);
	   		IT2->loadImageData(f2, cameraWidth, cameraHeight, GL_RGB); 
			if (!facegrabbed2 && chooseDetection) {
				facegrabbed2 = true;
				//make three copies for the different threads to operate on
				memcpy(img2, f2, camSize2);
				memcpy(img5, f2, camSize2);
				memcpy(c2, f2, camSize2);
				//set thread variables
				facedone2 = false;
				facedone5 = false;
			}
		} else if (count2 % 2 == 1) {
			VI.getPixels(dev+1, frame2, true, false);
	   		IT2->loadImageData(frame2, cameraWidth, cameraHeight, GL_RGB);
			if (!facegrabbed2 && chooseDetection) {
				//set our thread variable
				facegrabbed2 = true;
				//make three copies for the different threads to operate on
				memcpy(img2, frame2, camSize2);
				memcpy(img5, frame2, camSize2);
				memcpy(c2, frame2, camSize2);
				//set thread variables
				facedone2 = false;
				facedone5 = false;
			}
		}
		if (useThreads) {
			HANDLE myThread2;
			myThread2 = (HANDLE)_beginthread(procThread2, 0, (void *) &dev2);
		}
	}

	//check to see if we have got a new frame, for camera3 this time
	if( VI.isFrameNew(dev+2) )						
	{	
		count3++;
		//we alternate frames so we can take the difference between frames
		if (count3 % 2 == 0) {
			VI.getPixels(dev+2, f3, true, false);
	   		IT3->loadImageData(f3, cameraWidth, cameraHeight, GL_RGB);
			if (!facegrabbed3 && chooseDetection) {
				facegrabbed3 = true;
				//make copies for the different threads
				memcpy(img3, f3, camSize3);
				memcpy(img6, f3, camSize3);
				memcpy(c3, f3, camSize3);
				//set thread variables
				facedone3 = false;
				facedone6 = false;
			}
		} else if (count3 % 2 == 1) {
			VI.getPixels(dev+2, frame3, true, false);
	   		IT3->loadImageData(frame3, cameraWidth, cameraHeight, GL_RGB);
			if (!facegrabbed3 && chooseDetection) {
				facegrabbed3 = true;
				//create an extra image for the facedetection thread, and another for the triangulation
				//thread.
				memcpy(img3, frame3, camSize3);
				memcpy(img6, frame3, camSize3);
				memcpy(c3, frame3, camSize3);
				//set thread variables
				facedone3 = false;
				facedone6 = false;
			}
		}
		if (useThreads) {
			HANDLE myThread3;
			myThread3 = (HANDLE)_beginthread(procThread3, 0, (void *) &dev3);
		}
	}
	if( VI.isFrameNew(dev) || VI.isFrameNew(dev+1) || VI.isFrameNew(dev+2)) {
		if (alphaInit && betaInit && gammaInit) {
			mapDisplay->loadImageData(map, mapWidth, mapHeight, GL_RGB);
		}
	}
	//if anyone of the front faces aren't done and we aren't checking for profile faces
	//OR anyone of the front faces aren't done and any one of the profile faces aren't done
	if (!facedone1 && !facedone2 && !facedone3 && (!useProfile || (!facedone4 && !facedone5 && !facedone6))) {
		printf("Find Faces\n");
		//Set all our facefinding thread variables to true
		facedone1 = true;
		facedone2 = true;
		facedone3 = true;
		if (useProfile) {
			facedone4 = true;
			facedone5 = true;
			facedone6 = true;
		}
		//spawns 6 threads, 3 detect profile, 3 detect front of face
		findFace();
	}
	if (!facesdrawn1 && !facesdrawn2 && !facesdrawn3 && (!useProfile || (!facesdrawn4 && !facesdrawn5 && !facesdrawn6))) {
		printf("Drawing rectangles!!!\n");
		HANDLE draw1, draw2, draw3;
		facesdrawn1 = true;
		facesdrawn2 = true;
		facesdrawn3 = true;
		if (useProfile) {
			facesdrawn4 = true;
			facesdrawn5 = true;
			facesdrawn6 = true;
		}
		//spawn three threads to draw rectangles around the faces
		draw1 = (HANDLE) _beginthread(drawRectangles, 0, (void *) &dev);
		draw2 = (HANDLE) _beginthread(drawRectangles2, 0, (void *) &dev);
		draw3 = (HANDLE) _beginthread(drawRectangles3, 0, (void *) &dev);
	} 

	if (!facedisplayed1 && !facedisplayed2 && !facedisplayed3) {
		IplImage * pic = cvCreateImage(cvSize(640, 480), 8, 3);
		if (frontalFaceCount1>frontalFaceCount2 && frontalFaceCount1 > frontalFaceCount3) {
			pic->imageData = (char *) c1;
			cvRectangle(pic, topLeft, bottomRight, frameColor, 5);
		} else if (frontalFaceCount2>frontalFaceCount1 && frontalFaceCount2 > frontalFaceCount3) {
			pic->imageData = (char *) c2;
			cvRectangle(pic, topLeft, bottomRight, frameColor, 5);
		} else if (frontalFaceCount3>frontalFaceCount2 && frontalFaceCount3 > frontalFaceCount1) {
			pic->imageData = (char *) c3;
			cvRectangle(pic, topLeft, bottomRight, frameColor, 5);
		}

		facedisplayed1 = true;
		facedisplayed2 = true;
		facedisplayed3 = true;
		printf("Display Faces\n");
		i1->loadImageData( c1, 640, 480, GL_RGB);
		i2->loadImageData( c2, 640, 480, GL_RGB);
		i3->loadImageData( c3, 640, 480, GL_RGB);

		facegrabbed1 = false;
		facegrabbed2 = false;
		facegrabbed3 = false;
		printf("Exit Display Faces\n");
	}
}


void triangleApp::draw(){
  
	setupScreen();

	// Display Triangulation
	mapDisplay->renderTexture(640, 480, mapWidth, mapHeight);

	// Display Snapshots
	i1->renderTexture(0, 0, 640, 480);
	i2->renderTexture(640, 0, 640, 480);
	i3->renderTexture(1280, 0, 640, 480);

	// Display Video
	IT->renderTexture(0, 960, 640, 480);
	IT2->renderTexture(640, 960, 640, 480);
	IT3->renderTexture(1280, 960, 640, 480);
	

}

void triangleApp::keyDown  (char c){

	//some options hooked up to key commands
	if(c=='1')VI.showSettingsWindow(dev);
	if(c=='2')VI.showSettingsWindow(dev+1);
	if(c=='3')VI.showSettingsWindow(dev+2);

	if(c=='4')VI.restartDevice(dev);
	if(c=='5')VI.restartDevice(dev+1);
	if(c=='6')VI.restartDevice(dev+2);
	
	if (c=='P') {
		snapshot();
	} else if (c=='B') {
		getBackground();
	} else if (c=='S') {
		saveImage();
	} else if (c=='F') {
		findFace();
	} else if (c=='Q') {
		VI.stopDevice(dev);
		VI.stopDevice(dev+1);
		VI.stopDevice(dev+2);
	} else if (c=='L') {
		i1->loadImageData(img1, cameraWidth, cameraHeight, GL_RGB); 
		i2->loadImageData(img2, cameraWidth, cameraHeight, GL_RGB); 
		i3->loadImageData(img3, cameraWidth, cameraHeight, GL_RGB); 
	} else if (c=='C') {
		motionfilter = !motionfilter;
	} else if (c=='U') {
		useSkinFilter = !useSkinFilter;
	}
}

void triangleApp::snapshot() {

		printf("Taking picture...\n");

		// Take snapshot of images
		VI.getPixels(dev, c1, true, false);
		VI.getPixels(dev+1, c2, true, false);
		VI.getPixels(dev+2, c3, true, false);

   		i1->loadImageData(c1, cameraWidth, cameraHeight, GL_RGB);
		i2->loadImageData(c2, cameraWidth, cameraHeight, GL_RGB);
		i3->loadImageData(c3, cameraWidth, cameraHeight, GL_RGB);

		printf("Picture Taken...\n");
//		count++;
}


void triangleApp::findFace() {
//	printf("Finding faces...\n");		
//	printf("Faces found...\n");

	HANDLE find1, find2, find3, find4, find5, find6;

	find1 = (HANDLE) _beginthread(frontThread, 0, (void *) &i1);
	find2 = (HANDLE) _beginthread(frontThread2, 0, (void *) &i2);
	find3 = (HANDLE) _beginthread(frontThread3, 0, (void *) &i3);
	if (useProfile) {
		find4 = (HANDLE) _beginthread(profileThread, 0, (void *) &i1);
		find5 = (HANDLE) _beginthread(profileThread2, 0, (void *) &i2);
		find6 = (HANDLE) _beginthread(profileThread3, 0, (void *) &i3);
	}
}

void __cdecl triangleApp::frontThread(void * objPtr) {

    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    // Create a new image based on the input image
	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) img1;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
	// Allocate the memory storage
    storage = cvCreateMemStorage(0);
    
	// Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
	// Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        frontfaces1 = cvHaarDetectObjects( theImg, cascade, storage,
                                            1.05, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
	}
	cvReleaseImage(&theImg);
	facedone1 = true;
	facesdrawn1 = false;
//	facedisplayed1 = false;
	return;
}
void __cdecl triangleApp::frontThread2(void * objPtr) {
    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    // Create a new image based on the input image
	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) img2;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
	// Allocate the memory storage
    storage = cvCreateMemStorage(0);
    
	// Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
	// Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        frontfaces2 = cvHaarDetectObjects( theImg, cascade, storage,
                                            1.05, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
	}
	cvReleaseImage(&theImg);
	facedone2 = true;
	facesdrawn2 = false;
//	facedisplayed2 = false;
	return;
}
void __cdecl triangleApp::frontThread3(void * objPtr) {
    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    // Create a new image based on the input image
	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) img3;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
	// Allocate the memory storage
    storage = cvCreateMemStorage(0);
    
	// Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
	// Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        frontfaces3 = cvHaarDetectObjects( theImg, cascade, storage,
                                            1.05, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
	}
	cvReleaseImage(&theImg);
	facedone3 = true;
	facesdrawn3 = false;
//	facedisplayed3 = false;
	return;
}


void __cdecl triangleApp::profileThread(void * objPtr) {

    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    // Create a new image based on the input image
	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) img1;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name3, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
	// Allocate the memory storage
    storage = cvCreateMemStorage(0);
    
	// Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
	// Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {
        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        profilefaces1 = cvHaarDetectObjects( theImg, cascade, storage,
                                            1.05, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
	}
	cvReleaseImage(&theImg);
	facedone4 = true;
	facesdrawn4 = false;
//	facedisplayed1 = false;
	return;
}
void __cdecl triangleApp::profileThread2(void * objPtr) {
    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    // Create a new image based on the input image
	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) img2;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name3, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
	// Allocate the memory storage
    storage = cvCreateMemStorage(0);
    
	// Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
	// Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        profilefaces2 = cvHaarDetectObjects( theImg, cascade, storage,
                                            1.05, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
	}
	cvReleaseImage(&theImg);
	facedone5 = true;
	facesdrawn5 = false;
//	facedisplayed2 = false;
	return;
}

void __cdecl triangleApp::profileThread3(void * objPtr) {
    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    // Create a new image based on the input image
	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) img3;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name3, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }
	// Allocate the memory storage
    storage = cvCreateMemStorage(0);
    
	// Clear the memory storage which was used before
    cvClearMemStorage( storage );
    
	// Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        profilefaces3 = cvHaarDetectObjects( theImg, cascade, storage,
                                            1.05, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(30, 30) );
	}
	cvReleaseImage(&theImg);
	facedone6 = true;
	facesdrawn6 = false;
//	facedisplayed3 = false;
	return;
}

void __cdecl triangleApp::drawRectangles(void * objPtr) {

	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) c1;

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
	
	frontalFaceCount1 = 0;

    // Loop the number of front faces found.
    for( int i = 0; i < (frontfaces1 ? frontfaces1->total : 0); i++ )
    {
       // Create a new rectangle for drawing the face
        CvRect* r = (CvRect*)cvGetSeqElem( frontfaces1, i );
		if (r->height*r->width > 90000) continue;

        // Find the dimensions of the face,and scale it if necessary
		pt1.x = r->x;
	    pt2.x = (r->x+r->width);
		pt1.y = r->y;
        pt2.y = (r->y+r->height);
		if (motionfilter && (pt1.x < xLength.x || pt2.x > xLength.y)) continue;

		//Skin Filter the rectangle
		double totalprob = 0.0;
		if (useSkinFilter) {
			CvLineIterator iterator, iterator2;
			CvPoint one, two;
			one.x = r->x;
			one.y = r->y;
			two.x = r->x+r->width;
			two.y = r->y;
			int loop1 = cvInitLineIterator(theImg, one, two, &iterator, 8, 0);
			for (int h = 0; h < loop1; h++) {
				one.x = r->x+h;
				one.y = r->y;
				two.x = r->x+h;
				two.y = r->y+r->height;
				int loop2 = cvInitLineIterator( theImg, pt1, pt2, &iterator2, 8, 0 );
				for( int i = 0; i < loop2; i++ ){
					int sum = iterator.ptr[0] + iterator.ptr[1] + iterator.ptr[2];
					if (sum == 0) continue;
					double rdiff = iterator.ptr[2]*255/sum - meanr;
					double bdiff = iterator.ptr[0]*255/sum - meanb;

					double exponent = rdiff*rdiff*coa + rdiff*bdiff*cob + bdiff*rdiff*coc + bdiff*bdiff*cod;
					double result = exp(-.5 * (exponent/255));
					totalprob += result;	
					CV_NEXT_LINE_POINT(iterator);
				}
			}
		}

		if (!useSkinFilter || totalprob/(r->height*r->width) > .09) {
			if (useSkinFilter) printf("front1: Total Prob: %f\n\n", totalprob/(r->height*r->width));		
			frontalFaceCount1++;
	        cvRectangle( theImg, pt1, pt2, CV_RGB(255,255,0), 3, 8, 0 );
		}
    }
	if (useProfile) {
		// Loop the number of profile faces found.
		for( int i = 0; i < (profilefaces1 ? profilefaces1->total : 0); i++ )
		{
		   // Create a new rectangle for drawing the face
			CvRect* r = (CvRect*)cvGetSeqElem( profilefaces1, i );
			if (r->height*r->width > 90000) continue;

			// Find the dimensions of the face,and scale it if necessary
			pt1.x = r->x;
			pt2.x = (r->x+r->width);
			pt1.y = r->y;
			pt2.y = (r->y+r->height);
			if (motionfilter && (pt1.x < xLength.x || pt2.x > xLength.y)) continue;

			//Skin Filter???
			cvRectangle( theImg, pt1, pt2, CV_RGB(0,0,255), 3, 8, 0 );
		}
	}
		
	facedisplayed1 = false;
	cvReleaseImage(&theImg);
	return;
}

void __cdecl triangleApp::drawRectangles2(void * objPtr) {

	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) c2;

    // Create two points to represent the face locations
    CvPoint pt1, pt2;

	frontalFaceCount2 = 0;

    // Loop the number of front faces found.
    for( int i = 0; i < (frontfaces2 ? frontfaces2->total : 0); i++ )
    {
       // Create a new rectangle for drawing the face
        CvRect* r = (CvRect*)cvGetSeqElem( frontfaces2, i );
		if (r->height*r->width > 90000) continue;

        // Find the dimensions of the face,and scale it if necessary
		pt1.x = r->x;
	    pt2.x = (r->x+r->width);
		pt1.y = r->y;
        pt2.y = (r->y+r->height);
        if (motionfilter && (pt1.x < xLength2.x || pt2.x > xLength2.y)) continue;
		
		//Skin Filter the rectangle
		double totalprob = 0.0;
		if (useSkinFilter) {
			CvLineIterator iterator, iterator2;
			CvPoint one, two;
			one.x = r->x;
			one.y = r->y;
			two.x = r->x+r->width;
			two.y = r->y;
			int loop1 = cvInitLineIterator(theImg, one, two, &iterator, 8, 0);
			for (int h = 0; h < loop1; h++) {
				one.x = r->x+h;
				one.y = r->y;
				two.x = r->x+h;
				two.y = r->y+r->height;
				int loop2 = cvInitLineIterator( theImg, pt1, pt2, &iterator2, 8, 0 );
				for( int i = 0; i < loop2; i++ ){
					int sum = iterator.ptr[0] + iterator.ptr[1] + iterator.ptr[2];
					if (sum == 0) continue;
					double rdiff = iterator.ptr[2]*255/sum - meanr;
					double bdiff = iterator.ptr[0]*255/sum - meanb;

					double exponent = rdiff*rdiff*coa + rdiff*bdiff*cob + bdiff*rdiff*coc + bdiff*bdiff*cod;
					double result = exp(-.5 * (exponent/255));
					totalprob += result;	
					CV_NEXT_LINE_POINT(iterator);
				}
			}
		}
		if (!useSkinFilter || totalprob/(r->height*r->width) > .09) {
			if (useSkinFilter) printf("front2: Total Prob: %f\n\n", totalprob/(r->height*r->width));			
			frontalFaceCount2++;
			cvRectangle( theImg, pt1, pt2, CV_RGB(255,255,0), 3, 8, 0 );
		}
    }

	if (useProfile) {
		// Loop the number of profile faces found.
		for( int i = 0; i < (profilefaces2 ? profilefaces2->total : 0); i++ )
		{
		   // Create a new rectangle for drawing the face
			CvRect* r = (CvRect*)cvGetSeqElem( profilefaces2, i );
			if (r->height*r->width > 90000) continue;

			// Find the dimensions of the face,and scale it if necessary
			pt1.x = r->x;
			pt2.x = (r->x+r->width);
			pt1.y = r->y;
			pt2.y = (r->y+r->height);
			if (motionfilter && (pt1.x < xLength2.x || pt2.x > xLength2.y)) continue;
			
			//Skin Filter??		
			cvRectangle( theImg, pt1, pt2, CV_RGB(0,0,255), 3, 8, 0 );
		}
	}
	
	facedisplayed2 = false;
	cvReleaseImage(&theImg);
	return;
}

void __cdecl triangleApp::drawRectangles3(void * objPtr) {

	IplImage* theImg = cvCreateImage( cvSize(640, 480), 8, 3);
	theImg->imageData = (char *) c3;

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
	
	frontalFaceCount3 = 0;

    // Loop the number of front faces found.
    for( int i = 0; i < (frontfaces3 ? frontfaces3->total : 0); i++ )
    {
       // Create a new rectangle for drawing the face
        CvRect* r = (CvRect*)cvGetSeqElem( frontfaces3, i );
		if (r->height*r->width > 90000) continue;

        // Find the dimensions of the face,and scale it if necessary
		pt1.x = r->x;
	    pt2.x = (r->x+r->width);
		pt1.y = r->y;
        pt2.y = (r->y+r->height);
        if (motionfilter && (pt1.x < xLength3.x || pt2.x > xLength3.y)) continue;
		
		//Skin Filter the rectangle
		double totalprob = 0.0;
		if (useSkinFilter) {
			CvLineIterator iterator, iterator2;
			CvPoint one, two;
			one.x = r->x;
			one.y = r->y;
			two.x = r->x+r->width;
			two.y = r->y;
			int loop1 = cvInitLineIterator(theImg, one, two, &iterator, 8, 0);
			for (int h = 0; h < loop1; h++) {
				one.x = r->x+h;
				one.y = r->y;
				two.x = r->x+h;
				two.y = r->y+r->height;
				int loop2 = cvInitLineIterator( theImg, pt1, pt2, &iterator2, 8, 0 );
				for( int i = 0; i < loop2; i++ ){
					int sum = iterator.ptr[0] + iterator.ptr[1] + iterator.ptr[2];
					if (sum == 0) continue;
					double rdiff = iterator.ptr[2]*255/sum - meanr;
					double bdiff = iterator.ptr[0]*255/sum - meanb;

					double exponent = rdiff*rdiff*coa + rdiff*bdiff*cob + bdiff*rdiff*coc + bdiff*bdiff*cod;
					double result = exp(-.5 * (exponent/255));
					totalprob += result;	
					CV_NEXT_LINE_POINT(iterator);
				}
			}
		}

		if (!useSkinFilter || totalprob/(r->height*r->width) > .09) {
			if (useSkinFilter) printf("front3: Total Prob: %f\n\n", totalprob/(r->height*r->width));			
			frontalFaceCount3++;
			cvRectangle( theImg, pt1, pt2, CV_RGB(255,255,0), 3, 8, 0 );
		}
    }
	if (useProfile) {
		// Loop the number of profile faces found.
		for( int i = 0; i < (profilefaces3 ? profilefaces3->total : 0); i++ )
		{
		   // Create a new rectangle for drawing the face
			CvRect* r = (CvRect*)cvGetSeqElem( profilefaces3, i );
			if (r->height*r->width > 90000) continue;

			// Find the dimensions of the face,and scale it if necessary
			pt1.x = r->x;
			pt2.x = (r->x+r->width);
			pt1.y = r->y;
			pt2.y = (r->y+r->height);
			if (motionfilter && (pt1.x < xLength3.x || pt2.x > xLength3.y)) continue;
			
			//Skin Filter??
			cvRectangle( theImg, pt1, pt2, CV_RGB(0,0,255), 3, 8, 0 );
		}
	}
	
	cvReleaseImage(&theImg);
	facedisplayed3 = false;
	return;
}

void triangleApp::saveImage() {
		count++;
		printf("Saving Pictures");
		
		char buf[10];
		sprintf_s(buf, "%d", count);
		
		std::string str1 ("CAM1_");
		std::string str2 ("CAM2_");
		std::string str3 ("CAM3_");

		std::string suffix (".jpg");
   		
		str1.append(buf);
		str1.append(suffix);
		str2.append(buf);
		str2.append(suffix);
		str3.append(buf);
		str3.append(suffix);

		IplImage * s1 = cvCreateImage(cvSize(640,480), 8 , 3);
		IplImage * s2 = cvCreateImage(cvSize(640,480), 8 , 3);
		IplImage * s3 = cvCreateImage(cvSize(640,480), 8 , 3);

		VItoIPL(c1, s1);
		VItoIPL(c2, s2);
		VItoIPL(c3, s3);

		if(!cvSaveImage(str1.c_str(),s1)) printf("Could not save CAM1");
		if(!cvSaveImage(str2.c_str(),s2)) printf("Could not save CAM2");
		if(!cvSaveImage(str3.c_str(),s3)) printf("Could not save CAM3");
		cvReleaseImage(&s1);
		cvReleaseImage(&s2);
		cvReleaseImage(&s3);

		printf("Done Saving");

}

void triangleApp::VItoIPL(unsigned char * src, IplImage * dst) {
	unsigned char * temp = new unsigned char[camSize1];
	VI.processPixels(src, temp, 640, 480, true, true);
	dst->imageData = (char *) temp;
}

void triangleApp::printCoordinates(CvScalar color, CvPoint coord, CvScalar color2, CvPoint coord2) {
	IplImage * mapImg = cvCreateImage(cvSize(mapWidth, mapHeight), 8, 3);
	mapImg->imageData = (char *) map;
	cvZero(mapImg);
	cvCircle(mapImg, cam1, 10, cameraColor, 3);
	cvLine(mapImg, cam1, camDirection1, cameraColor, 3);
	cvCircle(mapImg, cam2, 10, cameraColor, 3);
	cvLine(mapImg, cam2, camDirection2, cameraColor, 3);
	cvCircle(mapImg, cam3, 10, cameraColor, 3);
	cvLine(mapImg, cam3, camDirection3, cameraColor, 3);
	cvCircle(mapImg, coord, 6, color, 2);
	cvCircle(mapImg, coord2, 6, color2, 2);
	cvFlip(mapImg, mapImg, 0);
	cvReleaseImage(&mapImg);
}

void triangleApp::triangulate(int x1, int y1, int x2, int y2, CvMat R, CvMat T) {
	double secondaryVector[]  = {(-x2+320)/focal, (-y2+240)/focal, 1};
	CvMat Line2 = cvMat(3, 1, CV_64FC1, secondaryVector);
	cvMatMul(&R, &Line2, &Line2);

	double v1 = (-x1+320)/focal;
	double w1 = cvmGet(&Line2,0,0);
	double w3 = cvmGet(&Line2,2,0);
	double T1 = cvmGet(&T,0,0);
	double T3 = cvmGet(&T,2,0);
	double factor = (T3*w1-T1*w3)/(w1-v1*w3);

	zpos = factor;
	xpos = factor * v1;
}

void triangleApp::putEverythingTogether() {

	CvScalar color = CV_RGB(255,0,0);
	CvScalar color2 = CV_RGB(0,0,255);
	
	//Combining frame 1 and frame 2
	triangulate(com1.x, com1.y, com2.x, com2.y, R21, T21);
	trianglePt.x = cvRound(zpos/mapScale);
	trianglePt.y = cvRound(-xpos/mapScale) + mapCenter.y;

	//Combining frame 2 and 3	
	triangulate(com2.x, com2.y, com3.x, com3.y, R32, T32);

	//Rotates to original frame
	double pos2[3] = {xpos, 0, zpos};
	CvMat Pos2 = cvMat(3, 1, CV_64FC1, pos2);
	cvMatMul(&R21, &Pos2, &Pos2);
	cvAdd(&T21, &Pos2, &Pos2);
	
	//Set second Triangle Point
	trianglePt2.x = cvRound(cvmGet(&Pos2,2,0) / mapScale);
	trianglePt2.y = cvRound(-cvmGet(&Pos2,0,0) / mapScale) + mapCenter.y;

	printCoordinates(color, trianglePt, color2, trianglePt2);
}

void triangleApp::getBackground() {
		printf("Taking background picture...\n");
		VI.getPixels(dev, b1, true, false);
		VI.getPixels(dev+1, b2, true, false);
		VI.getPixels(dev+2, b3, true, false);
		getBack = true;
}
