#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "../include/RaspiCam.h"

using namespace std; 

RaspiCam::RaspiCam():
	// Image Size
	imgWidth(320), imgHeight(240),

	// Border Width
	FBW(0),	LBW(0), RBW(0),

	// Correct Wall Area
	correctFrontWallArea(1900),
	correctLeftWallArea(1900),
	correctRightWallArea(1900),

	// Correct Wall Pt
	correctFrontWallStPt( 55, 50),	correctFrontWallMidPt(150, 55),	correctFrontWallEdPt(245, 50),
	correctLeftWallStPt ( 55, 50),	correctLeftWallMidPt ( 60,150),	correctLeftWallEdPt ( 55,240),
	correctRightWallStPt(245, 50),	correctRightWallMidPt(235,150),	correctRightWallEdPt(245,240),
	
	// Correct Mouse Pt
	correctMouseCenterPt(
					(correctRightWallMidPt.x+correctLeftWallMidPt.x)*0.5,
					(correctRightWallMidPt.y+correctLeftWallMidPt.y)*0.5),
	correctMouseFPt	(correctMouseCenterPt.x , correctMouseCenterPt.y - 54),
	correctMouseFLPt(correctMouseCenterPt.x - 35, correctMouseCenterPt.y - 40),
	correctMouseFRPt(correctMouseCenterPt.x + 50, correctMouseCenterPt.y - 40),

	// Pillar Area
	leftPillarPt (correctMouseCenterPt.x- 72, correctMouseCenterPt.y- 18),
	rightPillarPt(correctMouseCenterPt.x+102, correctMouseCenterPt.y- 25),
	pillarSize( 60, 60),

	// Correct Distance
	correctFrontDistance(correctMouseFPt.y - correctFrontWallMidPt.y),
	correctLeftDistance( correctMouseFLPt.x - correctLeftWallMidPt.x),
	correctRightDistance(correctRightWallMidPt.x - correctMouseFRPt.x),

	// Image Processing Area
	frontProcArea(cv::Size(	imgWidth-FBW*2,	imgHeight/2.6)),
	leftProcArea (cv::Size(	imgWidth/2.3, imgHeight-LBW)),
	rightProcArea(cv::Size(	imgWidth/2.3, imgHeight-RBW)),
	frontProcRect(cv::Rect(	FBW, 0, frontProcArea.width, frontProcArea.height)),	// Front	
	leftProcRect (cv::Rect(	0, LBW, leftProcArea.width,  leftProcArea.height)),		// Left
	rightProcRect(cv::Rect(	imgWidth-rightProcArea.width, RBW,						// Right
							rightProcArea.width, rightProcArea.height)),		
	frontWallArea(0), leftWallArea(0), rightWallArea(0)
{ }


// Destructor
RaspiCam::~RaspiCam()
{
	Camera.release();
}


// Camera Open
void RaspiCam::open()
{
	Camera.release();
	// Set camera params
	//Camera.set(CV_CAP_PROP_FPS, 60);
	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, imgWidth);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, imgHeight);
	Camera.set(CV_CAP_PROP_GAIN, 18);
	//Camera.set(CV_CAP_PROP_EXPOSURE, 80);
	
	// Open camera
	if (!Camera.open())
	{
		cerr<<"Error opening the camera"<<endl;
		return ;
	}
}


// Camera Close
void RaspiCam::close()
{
	Camera.release();
}

// Capture
void RaspiCam::capture()
{
	// Grab
	Camera.grab();
	Camera.retrieve(srcImg);

	// Flip
	cv::flip(srcImg, srcImg, -1);
}


// Create Window
void RaspiCam::createWindow()
{
	//cv::namedWindow("srcImg", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("frontRoi", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("leftRoi", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("rightRoi", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("frontBinImg", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("leftBinImg", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("rightBinImg", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("frontThinRoi", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("leftPillarRoi", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("rightPillarRoi", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("dstImg", CV_WINDOW_AUTOSIZE);
}


// Show Image
void RaspiCam::showImg()
{
	//cv::imshow("srcImg",srcImg);
	//cv::imshow("frontRoi",frontRoi);
	//cv::imshow("leftRoi",leftRoi);
	//cv::imshow("rightRoi",rightRoi);
	//cv::imshow("frontBinImg",frontBinImg);
	cv::imshow("leftBinImg",leftBinImg);
	cv::imshow("rightBinImg",rightBinImg);
	//cv::imshow("frontThinRoi",frontThinRoi);
	//cv::imshow("leftPillarRoi",leftPillarRoi);
	//cv::imshow("rightPillarRoi",rightPillarRoi);
	//cv::imshow("dstImg",dstImg);
	cv::waitKey(1);
}


// Write Image
void RaspiCam::writeImg()
{
	cv::imwrite("./Img/srcImg.jpg",srcImg);
}


// Calc Wall Center Point
int RaspiCam::calcSideWallCenter(cv::Mat img, cv::Point& center)
{
	int x, y;
	int area=0;
	int weightArea=0;
	double mx=0.0, my=0.0;
	for(y=0;y<img.rows;y++)
	{
		for(x=0;x<img.cols;x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0)
			{
				mx += x;
				my += y;
				area ++;
				if((y>=img.rows*0.0)&&(y<img.rows*0.3))
				{
					weightArea+=0;
				}
				else if((y>=img.rows*0.3)&&(y<img.rows*0.4))
				{
					weightArea+=1;
				}
				else if((y>=img.rows*0.4)&&(y<img.rows*0.7))
				{
					weightArea+=3;
				}
				else if((y>=img.rows*0.7)&&(y<img.rows*0.8))
				{
					weightArea+=1;
				}
				else if((y>=img.rows*0.8)&&(y<=img.rows*1.0))
				{
					weightArea+=0;
				}
			}
		}// x
	}//y
	center = cv::Point(mx/area, my/area);
	return weightArea;
}

int RaspiCam::calcFrontWallCenter(cv::Mat img, cv::Point& center)
{
	int x, y;
	int area=0;
	int weightArea=0;
	double mx=0.0, my=0.0;
	for(y=0;y<img.rows;y++)
	{
		for(x=0;x<img.cols;x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0)
			{
				mx += x;
				my += y;
				area ++;
				weightArea++;
				if((x>img.cols*0.4)&&(x<img.cols*0.6))
				{
					weightArea+=2;
				}
			}
		}// x
	}//y
	center = cv::Point(mx/area, my/area);
	return weightArea;
}


// Calc Nearest Wall Point
int RaspiCam::calcNearestFrontWallPt(cv::Mat img, cv::Point& nearest)
{
	nearest = cv::Point(0,0);
	int x, y;
	for(y=0;y<img.rows;y++)
	{
		for(x=0;x<img.cols;x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0 )
			{
				if((x<correctMouseFPt.x+10)&&(x>correctMouseFPt.x-10))
				{
					if(nearest.y < y){
						nearest = cv::Point(x,y);
					}
				}
			}
		}// x
	}//y
	return correctMouseFPt.y - nearest.y;
}

int RaspiCam::calcNearestLeftWallPt(cv::Mat img, cv::Point& nearest)
{
	nearest = cv::Point(0,0);
	int x, y;
	for(y=0;y<img.rows;y++)
	{
		for(x=0;x<img.cols;x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0 )
			{
				if((y<correctMouseFLPt.y+10)&&(y>correctMouseFLPt.y-10))
				{
					if(nearest.x < x){
						nearest = cv::Point(x,y);
					}
				}
			}
		}// x
	}//y
	return correctMouseFLPt.x - nearest.x;
}

int RaspiCam::calcNearestRightWallPt(cv::Mat img, cv::Point& nearest)
{
	nearest = cv::Point(img.cols,0);
	int x, y;
	int bias = imgWidth-img.cols;
	for(y=0;y<img.rows;y++)
	{
		for(x=0;x<img.cols;x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0 )
			{
				if((y<correctMouseFRPt.y+10)&&(y>correctMouseFRPt.y-10))
				{
					if(nearest.x > x){
						nearest = cv::Point(x,y);
					}
				}
			}
		}// x
	}//y
	return nearest.x - (correctMouseFRPt.x - bias);
}


// Thinning
void thinningIte(cv::Mat& img, int pattern)
{
	cv::Mat del_marker = cv::Mat::ones(img.size(), CV_8UC1);
	int x, y;
	for (y = 1; y < img.rows-1; ++y) 
	{
        	for (x = 1; x < img.cols-1; ++x) 
		{
			int v9,v2,v3;
			int v8,v1,v4;
			int v7,v6,v5;
			v1=img.data[   y   * img.step +   x   * img.elemSize()];
			v2=img.data[ (y-1) * img.step +   x   * img.elemSize()];
			v3=img.data[ (y-1) * img.step + (x+1) * img.elemSize()];
			v4=img.data[   y   * img.step + (x+1) * img.elemSize()];
			v5=img.data[ (y+1) * img.step + (x+1) * img.elemSize()];
			v6=img.data[ (y+1) * img.step +   x   * img.elemSize()];
			v7=img.data[ (y+1) * img.step + (x-1) * img.elemSize()];
			v8=img.data[   y   * img.step + (x-1) * img.elemSize()];
			v9=img.data[ (y-1) * img.step + (x-1) * img.elemSize()];
            		int S = (v2 == 0 && v3 == 1) + (v3 == 0 && v4 == 1) +
				(v4 == 0 && v5 == 1) + (v5 == 0 && v6 == 1) +
				(v6 == 0 && v7 == 1) + (v7 == 0 && v8 == 1) +
				(v8 == 0 && v9 == 1) + (v9 == 0 && v2 == 1);
			int N  = v2 + v3 + v4 + v5 + v6 + v7 + v8 + v9;
			int m1=0, m2=0;
			if(pattern==0) m1 = (v2 * v4 * v6);
			if(pattern==1) m1 = (v2 * v4 * v8);
			if(pattern==0) m2 = (v4 * v6 * v8);
			if(pattern==1) m2 = (v2 * v6 * v8);
			if (S == 1 && (N >= 2 && N <= 6) && m1 == 0 && m2 == 0)
				del_marker.data[y * del_marker.step + x * del_marker.elemSize()]=0;
		}
	}
	img &= del_marker;
}

void thinning(const cv::Mat& src, cv::Mat& dst)
{
	dst = src.clone();
	dst /= 255;         // 0 >> 0, over 1 >> 1
	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
	cv::Mat diff;
	do {
		thinningIte(dst, 0);
		thinningIte(dst, 1);
		cv::absdiff(dst, prev, diff);
		dst.copyTo(prev);
	}while (cv::countNonZero(diff) > 0);
	dst *= 255;
}


// Calc Front Wall Gradient
int calcFrontWallGradient(cv::Mat& img)
{
	int minY = 999999;
	int maxY = 0;
	cv::Point lineSt(0,0);
	cv::Point lineEd(0,0);
	double degree =0.0;
	
	for(int y=1; y<img.rows-1; y++)
	{
		for(int x=1; x<img.cols-1; x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0 && y < minY)
			{
				lineSt = cv::Point(x, y);
				minY = y;
			}
			if(img.data[y*img.step+x*img.elemSize()] != 0 && y > maxY)
			{
				lineEd = cv::Point(x, y);
				maxY = y;
			}
		}
	}
	cv::line(img, lineSt, lineEd, 128);
	int diffx = lineEd.x - lineSt.x;
	int diffy = lineEd.y - lineSt.y;

	if(diffx==0 || diffy==0)
	{
		degree = 0;
	}
	else 
	{
		degree = (-1.0) * atan2(diffy, diffx) * (180.0/M_PI); 
	}
	
	if(degree > 90)
	{
		degree -= 180.0;
	}
	else if(degree < -90)
	{
		degree += 180.0;
	}
	
	return (int)(degree+0.5);
}


// Calc Side Wall Gradient
int calcSideWallGradient(cv::Mat& img)
{
	int minX = 999999;
	int maxX = 0;
	cv::Point lineSt(0,0);
	cv::Point lineEd(0,0);
	double degree =0.0;
	
	for(int y=1; y<img.rows-1; y++)
	{
		for(int x=1; x<img.cols-1; x++)
		{
			if(img.data[y*img.step+x*img.elemSize()] != 0 && x < minX)
			{
				lineSt = cv::Point(x, y);
				minX = x;
			}
			if(img.data[y*img.step+x*img.elemSize()] != 0 && x > maxX)
			{
				lineEd = cv::Point(x, y);
				maxX = x;
			}
		}
	}
	cv::line(img, lineSt, lineEd, 128);
	int diffx = lineEd.x - lineSt.x;
	int diffy = lineEd.y - lineSt.y;
	if(diffx==0 || diffy==0)
	{
		degree = 0;
	}
	else 
	{
		degree = atan2(diffx, diffy) * (180.0/M_PI); 
	}
	
	if(degree > 90)
	{
		degree -= 180.0;
	}
	else if(degree < -90)
	{
		degree += 180.0;
	}
	
	return (int)(degree+0.5);
}


// Get Wall Data
void RaspiCam::getFrontWallData(int *wall, EstimatedErrors *mouseErr)
{
	// Roi
	cv::Mat tmpFrontRoi = srcImg(frontProcRect);
	frontRoi = tmpFrontRoi.clone();

	// BGR to HSV
	cv::cvtColor(frontRoi, frontRoi, CV_BGR2HSV);
	
	// Red Light filter
	cv::inRange(frontRoi, cv::Scalar(150, 75, 30), cv::Scalar(195, 255, 255), frontBinImg);
	
	// Morphology(Opening)
	cv::erode(frontBinImg, frontBinImg, cv::Mat(), cv::Point(-1, -1), 3);
	cv::dilate(frontBinImg, frontBinImg, cv::Mat(), cv::Point(-1, -1), 3);
	
	//
	// Estimate Position & Rotation
	//
	int measureFrontDistance = 0, measureFrontDegree = 0;
	measureFrontDistance = calcNearestFrontWallPt(frontBinImg, nearestFrontWallPt);
	mouseErr->y = measureFrontDistance - correctFrontDistance;

	// Calc Left Wall Gradient
	if(wall[0]==0 && wall[1]==1 && wall[2]==0)
	{
		// Thinning
		cv::Mat tmpFrontBinRoi = frontBinImg(cv::Rect(nearestFrontWallPt.x, 0, pillarSize.width, frontBinImg.rows));
		frontThinRoi = tmpFrontBinRoi.clone();
		thinning(frontThinRoi, frontThinRoi);
		mouseErr->degree = calcFrontWallGradient(frontThinRoi);
	}

	// DEBUG
	//cout << "nearestFrontWallPt:" << nearestFrontWallPt <<endl;
}


void RaspiCam::getSideWallData(int *wall, EstimatedErrors *mouseErr)
{
	//
	// Detect Side Wall
	//
	// Roi
	cv::Mat tmpLeftRoi = srcImg(leftProcRect);
	cv::Mat tmpRightRoi = srcImg(rightProcRect);
	leftRoi  = tmpLeftRoi.clone();
	rightRoi = tmpRightRoi.clone();
	
	// BGR to HSV
	cv::cvtColor(leftRoi, leftRoi, CV_BGR2HSV);
	cv::cvtColor(rightRoi, rightRoi, CV_BGR2HSV);

	// Red Light filter
	cv::inRange(leftRoi, cv::Scalar(150, 75, 30), cv::Scalar(195, 255, 255), leftBinImg);
	cv::inRange(rightRoi, cv::Scalar(150, 75, 30), cv::Scalar(195, 255, 255), rightBinImg);

	// Morphology(Opening)
	cv::erode(leftBinImg, leftBinImg, cv::Mat(), cv::Point(-1, -1), 3);
	cv::dilate(leftBinImg, leftBinImg, cv::Mat(), cv::Point(-1, -1), 3);
	cv::erode(rightBinImg, rightBinImg, cv::Mat(), cv::Point(-1, -1), 3);
	cv::dilate(rightBinImg, rightBinImg, cv::Mat(), cv::Point(-1, -1), 3);
	
	// Calc Wall Center
	leftWallArea	= calcSideWallCenter(leftBinImg, leftWallCenterPt);
	rightWallArea	= calcSideWallCenter(rightBinImg, rightWallCenterPt);
	leftWallCenterPt = cv::Point(leftWallCenterPt.x,leftWallCenterPt.y+LBW); 
	rightWallCenterPt = cv::Point(rightWallCenterPt.x+(imgWidth-rightRoi.cols),rightWallCenterPt.y+RBW); 
	if(leftWallArea  >= correctLeftWallArea) { wall[0] = 1; }
	if(rightWallArea >= correctRightWallArea){ wall[2] = 1; }

	//
	// Estimate Position & Rotation
	//
	int measureLeftDistance = 0, measureLeftDegree = 0;
	int measureRightDistance = 0, measureRightDegree = 0;
	
	// Roi Around Pillar
	cv::Mat tmpLeftPillarRoi	= leftBinImg(cv::Rect(0, leftPillarPt.y-(pillarSize.height*0.5), leftBinImg.cols, pillarSize.height));
	cv::Mat tmpRightPillarRoi	= rightBinImg(cv::Rect(0, rightPillarPt.y-(pillarSize.height*0.5), leftBinImg.cols, pillarSize.height));
	leftPillarRoi  = tmpLeftPillarRoi.clone();
	rightPillarRoi = tmpRightPillarRoi.clone();

	if(wall[0] == 0 && wall[2] == 0)
	{
		mouseErr->x = 0;
		mouseErr->degree = 0;
	}
	else if(wall[0] == 1 && wall[2] == 0)
	{
		// Thinning
		thinning(leftPillarRoi, leftPillarRoi);
		// Calc Left Wall Gradient
		measureLeftDistance = calcNearestLeftWallPt(leftBinImg, nearestLeftWallPt);
		measureLeftDegree = calcSideWallGradient(leftPillarRoi);
		mouseErr->x = correctLeftDistance - measureLeftDistance;
		mouseErr->degree = measureLeftDegree; 
	}
	else if(wall[0] == 0 && wall[2] == 1)
	{
		// Thinning
		thinning(rightPillarRoi, rightPillarRoi);
		// Calc Right Wall Gradient
		measureRightDistance = calcNearestRightWallPt(rightBinImg, nearestRightWallPt);
		measureRightDegree = calcSideWallGradient(rightPillarRoi);
		mouseErr->x = measureRightDistance - correctRightDistance;
		mouseErr->degree = measureRightDegree; 
	}
	else if(wall[0] == 1 && wall[2] == 1)
	{
		// Thinning
		thinning(leftPillarRoi, leftPillarRoi);
		thinning(rightPillarRoi, rightPillarRoi);
		// Calc Left Wall Gradient
		measureLeftDistance = calcNearestLeftWallPt(leftBinImg, nearestLeftWallPt);
		measureRightDistance = calcNearestRightWallPt(rightBinImg, nearestRightWallPt);
		if(measureLeftDistance < measureRightDistance)
		{
			mouseErr->x = correctLeftDistance - measureLeftDistance;
			mouseErr->degree = calcSideWallGradient(leftPillarRoi);
		}
		else
		{
			mouseErr->x = measureRightDistance - correctRightDistance;
			mouseErr->degree = calcSideWallGradient(rightPillarRoi);
		}
	}
	
	// DEBUG
	dstImg = srcImg.clone();
	cv::circle(dstImg, correctMouseCenterPt, 5, cv::Scalar(0,0,255), -1);
	cv::circle(dstImg, correctMouseFPt, 5, cv::Scalar(0,0,255), -1);
	cv::circle(dstImg, correctMouseFLPt, 5, cv::Scalar(0,0,255), -1);
	cv::circle(dstImg, correctMouseFRPt, 5, cv::Scalar(0,0,255), -1);
	
	cv::circle(dstImg, correctFrontWallMidPt, 5, cv::Scalar(0,255,255), -1);
	cv::circle(dstImg, correctLeftWallMidPt, 5, cv::Scalar(0,255,255), -1);
	cv::circle(dstImg, correctRightWallMidPt, 5, cv::Scalar(0,255,255), -1);
	
	if(wall[0] == 1){ cv::circle(dstImg, leftWallCenterPt, 5, cv::Scalar(0,255,0), -1);}
	if(wall[2] == 1){ cv::circle(dstImg, rightWallCenterPt, 5, cv::Scalar(0,255,05), -1);}
	cv::Point estMousePt;
	estMousePt.x = (rightWallCenterPt.x+leftWallCenterPt.x) * 0.5;
	estMousePt.y = (rightWallCenterPt.y+leftWallCenterPt.y) * 0.5;
	cv::circle(dstImg, estMousePt, 5, cv::Scalar(0,255,0), -1);
	cv::circle(dstImg, leftPillarPt, 5, cv::Scalar(255,0,0), -1);
	cv::circle(dstImg, rightPillarPt, 5, cv::Scalar(255,0,0), -1);
	
	/*
	cout << "wall[0]:" << wall[0] << " Area:" << leftWallArea	<< " leftWallCenterPt" << leftWallCenterPt << endl;
	cout << "wall[2]:" << wall[2] << " Area:" << rightWallArea	<< " rightWallCenterPt" << rightWallCenterPt << endl;
	cout << "measureLeftDistance:" << measureLeftDistance <<endl;
	cout << "measureRightDistance:" << measureRightDistance <<endl;
	
	cout << "mouseErr->x:" << mouseErr->x <<endl;
	cout << "mouseErr->y:" << mouseErr->y <<endl;
	cout << "mouseErr->degree:" << mouseErr->degree <<endl;
	//*/
	// DEBUG
}
