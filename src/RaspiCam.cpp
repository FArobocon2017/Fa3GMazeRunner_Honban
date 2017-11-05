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
	FBW(imgWidth/4.4),LBW(imgHeight/4),RBW(imgHeight/4),

	// Correct Wall Area
	correctFrontWallArea(1700),
	correctLeftWallArea(1500),
	correctRightWallArea(1500),

	// Image Processing Area
	frontProcArea(cv::Size(imgWidth-FBW*2,	imgHeight/2.6)),
	leftProcArea(cv::Size(imgWidth/2.3, imgHeight-LBW)),
	rightProcArea(cv::Size(imgWidth/2.3, imgHeight-RBW)),
	frontProcRect(cv::Rect(FBW-15, 0, frontProcArea.width, frontProcArea.height)),	// Front	
	leftProcRect(cv::Rect(0, LBW, leftProcArea.width,  leftProcArea.height)),	// Left
	rightProcRect(cv::Rect(imgWidth-rightProcArea.width, RBW,
				rightProcArea.width, rightProcArea.height)),		// Right
	frontWallArea(0), leftWallArea(0), rightWallArea(0),

	// Detection Parameter
	rotNum(120), rotAngle(3), threshold(0.85),
	
	// Correct Wall Pt
	correctFrontWallStPt(50,50), correctFrontWallMidPt(145,50), correctFrontWallEdPt(240,50),
	correctLeftWallStPt(50,50), correctLeftWallMidPt(50,145), correctLeftWallEdPt(50,240),
	correctRightWallStPt(240,50), correctRightWallMidPt(240,145), correctRightWallEdPt(240,240)
{ }


// Camera Close
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
	Camera.set(CV_CAP_PROP_GAIN, 20);
	//Camera.set(CV_CAP_PROP_EXPOSURE, 50);
	
	// Open camera
	if (!Camera.open())
	{
		cerr<<"Error opening the camera"<<endl;
		return ;
	}
}


// Create Window
void RaspiCam::createWindow()
{
	cv::namedWindow("srcImg", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("hsvImg", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("maskImg", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("thinImg", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("lsdImg", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("dstImg", CV_WINDOW_AUTOSIZE);
}


// Show Image
void RaspiCam::showImg()
{
	cv::imshow("srcImg",srcImg);
	cv::imshow("hsvImg",hsvImg);
	cv::imshow("maskImg",maskImg);
	//cv::imshow("thinImg",thinImg);
	//cv::imshow("lsdImg",lsdImg);
	//cv::imshow("dstImg",dstImg);
	cv::waitKey(1);
}


// Write Image
void RaspiCam::writeImg()
{
	cv::imwrite("./Img/srcImg.jpg",srcImg);
	cv::imwrite("./Img/hsvImg.jpg",hsvImg);
	cv::imwrite("./Img/maskImg.jpg",maskImg);
	cv::imwrite("./Img/thinImg.jpg",thinImg);
	//cv::imwrite("./Img/lsdImg.jpg",lsdImg);
	cv::imwrite("./Img/dstImg.jpg",dstImg);
}


// Train Image
void RaspiCam::createTrainImg(int pattern)
{
	// Capture
	cv::Mat capImg;
	Camera.grab();
	Camera.retrieve(capImg);

	// Flip
	cv::flip(capImg, capImg, -1);

	// BGR to HSV
	cv::cvtColor(capImg, hsvImg, CV_BGR2HSV);

	// Red Light filter
	cv::inRange(hsvImg, cv::Scalar(150, 60, 70), cv::Scalar(190, 255, 255), maskImg);

	// Morphology(Opening)
	morphologyEx(maskImg, morphoImg, cv::MORPH_OPEN, cv::Mat(3, 3, CV_8UC1), cv::Point(-1, -1), 3, cv::BORDER_REFLECT);
	
	// Write Image
	stringstream fileName;
	cv::Mat cutImg(morphoImg, cv::Rect(179, 65, 44, 44));
	
	// L
	if(pattern == 0)
	{
		fileName << "./train/L.jpg";
		cv::imwrite(fileName.str(), cutImg);
		cout << "writeImg:" << fileName.str() << endl;
	}
	// T
	else if(pattern == 1)
	{
		fileName << "./train/T.jpg";
		cv::imwrite(fileName.str(), cutImg);
		cout << "writeImg:" << fileName.str() << endl;
	}
	// Cross
	else if(pattern == 2)
	{
		fileName << "./train/Cross.jpg";
		cv::imwrite(fileName.str(), cutImg);
		cout << "writeImg:" << fileName.str() << endl;
	}
	cv::imwrite("./Img/capImg.jpg", capImg);
	cv::imwrite("./Img/hsvImg.jpg", hsvImg);
	cv::imwrite("./Img/maskImg.jpg", maskImg);
	cv::imwrite("./Img/morphoImg.jpg", morphoImg);
}


// Load Train Image
void RaspiCam::loadTrainImg()
{
	trainLImg = cv::imread("./train/L.jpg",-1);
	trainTImg = cv::imread("./train/T.jpg",-1);
	trainCrossImg = cv::imread("./train/Cross.jpg",-1);
	cout << "Load Train Img" << endl;
}


// Detect Corner
int RaspiCam::detectCorner(cv::Mat inImg, cv::Mat trainImg, cv::Point& retPt)
{
	double maxScore = 0.0;
	int maxIndex;
	cv::Mat rotImg;
	cv::Mat scoreMat;
	cv::Point2d center(trainImg.cols*0.5, trainImg.rows*0.5);
	for(int rotIndex = 0; rotIndex < rotNum; rotIndex++)
	{
		cv::Mat H = getRotationMatrix2D(center, rotIndex * rotAngle, 1);
		cv::warpAffine(inImg, rotImg, H, inImg.size());
		cv::matchTemplate(rotImg, trainImg, scoreMat, CV_TM_CCOEFF_NORMED);
		// Search Maximum Score
		cv::Point maxPt;
		double maxVal;
		cv::minMaxLoc(scoreMat, NULL, &maxVal, NULL, &maxPt);
		if ( maxVal > maxScore )
		{
			maxScore = maxVal;
			maxIndex = rotIndex;
			retPt = maxPt;
		}
	}
	if(maxScore < threshold)
	{
		return -1;
	}
	return maxScore;
}


// Calc Center
int calcCenter(cv::Mat img, cv::Point& center)
{
	int x, y;
	int area=0;
	double mx=0.0, my=0.0;
	for(y=0;y<img.rows;y++)
	{
		for(x=0;x<img.cols;x++)
		{
			if(img.data[   y   * img.step +   x   * img.elemSize()] != 0)
			{
				mx += x;
				my += y;
				area ++;
			}
		}// x
	}//y
	center = cv::Point(mx/area, my/area);
	return area;
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


// Get Wall Data
void RaspiCam::getWallData(int *wall, EstimatedErrors *mouseErr)
{
	// Capture
	Camera.grab();
	Camera.retrieve(srcImg);
	
	// Flip
	cv::flip(srcImg, srcImg, -1);

	// BGR to HSV
	cv::cvtColor(srcImg, hsvImg, CV_BGR2HSV);

	// Red Light filter
	cv::inRange(hsvImg, cv::Scalar(150, 70, 70), cv::Scalar(190, 255, 255), maskImg);
	
	// Morphology(Opening)
	morphologyEx(maskImg, morphoImg, cv::MORPH_OPEN, cv::Mat(3, 3, CV_8UC1), cv::Point(-1, -1), 3, cv::BORDER_REFLECT);
	
/*
	// ROI
	cv::Mat frontRoi = morphoImg(frontProcRect);
	cv::Mat leftRoi  = morphoImg(leftProcRect);
	cv::Mat rightRoi = morphoImg(rightProcRect);
	cout << "Roi" << endl;

	// Calc Wall Center
	frontWallArea	= calcCenter(frontRoi, frontWallCenter);
	leftWallArea	= calcCenter(leftRoi, leftWallCenter);
	rightWallArea	= calcCenter(rightRoi, rightWallCenter);
	frontWallCenter = cv::Point(frontWallCenter.x+FBW-15,frontWallCenter.y); 
	leftWallCenter 	= cv::Point(leftWallCenter.x,leftWallCenter.y+LBW); 
	rightWallCenter = cv::Point(rightWallCenter.x+(morphoImg.cols-rightRoi.cols),rightWallCenter.y+RBW); 
	if(leftWallArea  >= correctLeftWallArea) { wall[0] = 1; }
	if(frontWallArea >= correctFrontWallArea){ wall[1] = 1; }
	if(rightWallArea >= correctRightWallArea){ wall[2] = 1; }
	cout << "Roi" << endl;
	cout << "frontWallArea" << frontWallArea << endl;
	cout << "leftWallArea" << leftWallArea << endl;
	cout << "rightWallArea" <<rightWallArea<< endl;
	// Estimate Position & Rotation
	cv::Point roiPt(0,0);
	int minX = 999999;
	int maxX = 0;
	int minY = 999999;
	int maxY = 0;
	cv::Point lineSt(0,0);
	cv::Point lineEd(0,0);
	if( wall[0]==0 && wall[1]==0 && wall[2]==0)
	{
		// no action
	}
	else if( (wall[0]==1 && wall[1]==0 && wall[2]==0) ||
	    (wall[0]==0 && wall[1]==1 && wall[2]==0) ||
	    (wall[0]==0 && wall[1]==0 && wall[2]==1) )
	{
		cout << "wall 1" << endl;
		if(wall[0]==1)
		{
			cout << "wall 2" << endl;
			if(leftWallCenter.x-20 > 0)
			{

				roiPt.x = leftWallCenter.x-20;
			}
			if(leftWallCenter.y-20 > 0)
			{
				roiPt.y = leftWallCenter.y-20;
			}
			cv::Mat roi(morphoImg, cv::Rect(roiPt.x, roiPt.y, 40, 40));
			thinning(roi, thinImg);
			for(int y=0; y<thinImg.rows; y++)
			{
				for(int x=0; x<thinImg.cols; x++)
				{
					if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y < minY)
					{
						lineSt = cv::Point(x, y);
						minY = y;
					}
					if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y > maxY)
					{
						lineEd = cv::Point(x, y);
						maxY = y;
					}
				}
			}
			double degree = atan2(lineEd.y - lineSt.y, lineEd.x - lineSt.x) * (180.0/M_PI);
			
			mouseErr->x = leftWallCenter.x - correctLeftWallMidPt.x;
			mouseErr->y = leftWallCenter.y - correctLeftWallMidPt.y;
			mouseErr->degree = (int)(degree+0.5);
		}
		else if(wall[1]==1)
		{
			cout << "wall 3" << endl;
			if(frontWallCenter.x-20 > 0)
			{
				roiPt.x = frontWallCenter.x-20;
			}
			if(frontWallCenter.y-20 > 0)
			{
				roiPt.y = frontWallCenter.y-20;
			}
			cv::Mat roi(morphoImg, cv::Rect(roiPt.x, roiPt.y, 40, 40));
			cout << "wall1 roi" << endl;
			thinning(roi, thinImg);
			cout << "wall1 thin" << endl;
			for(int y=0; y<thinImg.rows; y++)
			{
				for(int x=0; x<thinImg.cols; x++)
				{
					if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && x < minX)
					{
						lineSt = cv::Point(x, y);
						minX = x;
					}
					if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && x > maxX)
					{
						lineEd = cv::Point(x, y);
						maxX = x;
					}
				}
			}
			cout << "wall2" << endl;
			double degree = atan2(lineEd.y - lineSt.y, lineEd.x - lineSt.x) * (180.0/M_PI);
			
			mouseErr->x = frontWallCenter.x - correctFrontWallMidPt.x;
			mouseErr->y = frontWallCenter.y - correctFrontWallMidPt.y;
			mouseErr->degree = (int)(degree+0.5);
		}
		else if(wall[2]==1)
		{
			cout << "wall 4" << endl;
			if(rightWallCenter.x+20 < morphoImg.cols)
			{
				roiPt.x = rightWallCenter.x-20;
			}
			if(rightWallCenter.y-20 > 0)
			{
				roiPt.y = rightWallCenter.y-20;
			}
			cv::Mat roi(morphoImg, cv::Rect(roiPt.x, roiPt.y, 40, 40));
			thinning(roi, thinImg);
			for(int y=0; y<thinImg.rows; y++)
			{
				for(int x=0; x<thinImg.cols; x++)
				{
					if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y < minY)
					{
						lineSt = cv::Point(x, y);
						minY = y;
					}
					if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y > maxY)
					{
						lineEd = cv::Point(x, y);
						maxY = y;
					}
				}
			}
			double degree = atan2(lineEd.y - lineSt.y, lineEd.x - lineSt.x) * (180.0/M_PI);
			
			mouseErr->x = rightWallCenter.x - correctRightWallMidPt.x;
			mouseErr->y = rightWallCenter.y - correctRightWallMidPt.y;
			mouseErr->degree = (int)(degree+0.5);
		}		
	}
	else if( wall[0]==1 && wall[1]==1 && wall[2]==1 )
	{
		cout << "wall 5" << endl;
		vector<cv::Point2f> src, dst;
		src.push_back(cv::Point2f(leftWallCenter.x, leftWallCenter.y));
		src.push_back(cv::Point2f(frontWallCenter.x, frontWallCenter.y));
		src.push_back(cv::Point2f(rightWallCenter.x, rightWallCenter.y));
		dst.push_back(cv::Point2f(correctLeftWallMidPt.x, correctLeftWallMidPt.y));
		dst.push_back(cv::Point2f(correctFrontWallMidPt.x, correctFrontWallMidPt.y));
		dst.push_back(cv::Point2f(correctRightWallMidPt.x, correctRightWallMidPt.y));
		
		cv::Mat transMat = cv::getAffineTransform(src, dst);
		double theta = transMat.at<double>(0, 0);
		double degree = acos(theta) * (180.0/M_PI);
		mouseErr->x = transMat.at<double>(0, 2);
		mouseErr->y = transMat.at<double>(1, 2);
		mouseErr->degree = (int)(degree+0.5);
		// DEBUG
		cout << theta <<endl;
		cout << degree <<endl;
		cout << mouseErr->x <<endl;
		cout << mouseErr->degree <<endl;
	}
	else if( wall[1] != 1 )
	{
		cout << "wall 6" << endl;
		if(leftWallCenter.x-20 > 0)
		{
			roiPt.x = leftWallCenter.x-20;
		}
		if(leftWallCenter.y-20 > 0)
		{
			roiPt.y = leftWallCenter.y-20;
		}
		cv::Mat roi(morphoImg, cv::Rect(roiPt.x, roiPt.y, 40, 40));
		thinning(roi, thinImg);
		for(int y=0; y<thinImg.rows; y++)
		{
			for(int x=0; x<thinImg.cols; x++)
			{
				if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y < minY)
				{
					lineSt = cv::Point(x, y);
					minY = y;
				}
				if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y > maxY)
				{
					lineEd = cv::Point(x, y);
					maxY = y;
				}
			}
		}
		double degree = atan2(lineEd.y - lineSt.y, lineEd.x - lineSt.x) * (180.0/M_PI);
			
		// Ignore Y error
		mouseErr->x = (correctRightWallMidPt.x - correctLeftWallMidPt.x)*0.5 - (rightWallCenter.x - leftWallCenter.x)*0.5;
		mouseErr->y = 0;
		mouseErr->degree = (int)(degree+0.5);
	}
	else
	{
		cout << "wall 7" << endl;
		cv::Point maxRPt, maxLPt;
		int scoreR, scoreL;
		int maxScoreL= -1, maxScoreR = -1;
		int score[6] = {-1};		// RT, RL, RCross, LT, LL, LCross
		vector<cv::Point> retPt;	// RT, RL, RCross, LT, LL, LCross
		// Search Corner Proc
		for(int y=0;y< (morphoImg.rows*0.5)-(trainLImg.rows);y++)
		{
			for(int x=0;x<(morphoImg.cols*0.5) - (trainLImg.cols);x++)
			{
				cv::Mat roiL(morphoImg, cv::Rect(x, y, trainLImg.cols, trainLImg.rows));
				cv::Mat roiR(morphoImg, cv::Rect(x+morphoImg.cols*0.5, y, trainLImg.cols, trainLImg.rows));				
				score[0] = detectCorner(roiR, trainTImg, retPt[0]);
				score[3] = detectCorner(roiL, trainTImg, retPt[3]);
				score[1] = detectCorner(roiR, trainLImg, retPt[1]);
				score[4] = detectCorner(roiL, trainLImg, retPt[4]);
				score[2] = detectCorner(roiR, trainCrossImg, retPt[2]);
				score[5] = detectCorner(roiL, trainCrossImg, retPt[5]);
				// Calc Max Score
				for(int i=0; i<3; i++)
				{
					// R
					if(score[i]>maxScoreR && score[i]>threshold)
					{
						maxScoreR = score[i];
						maxRPt = retPt[i];
					}
					// L
					if(score[i+3]>maxScoreR && score[i+3]>threshold)
					{
						maxScoreL = score[i+3];
						maxLPt = retPt[i+3];
					}
				}
				
			}
		}		
		if(maxScoreR > 0 || maxScoreL> 0)
		{
			cout << "ms"  << endl;
			if(wall[0]==1)
			{
				if(leftWallCenter.x-20 > 0)
				{
					roiPt.x = leftWallCenter.x-20;
				}
				if(leftWallCenter.y-20 > 0)
				{
					roiPt.y = leftWallCenter.y-20;
				}
				cout << "wall7roi"  << endl;
				cv::Mat roi(morphoImg, cv::Rect(roiPt.x, roiPt.y, 40, 40));
				thinning(roi, thinImg);
				for(int y=0; y<thinImg.rows; y++)
				{
					for(int x=0; x<thinImg.cols; x++)
					{
						if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y < minY)
						{
							lineSt = cv::Point(x, y);
							minY = y;
						}
						if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y > maxY)
						{
							lineEd = cv::Point(x, y);
							maxY = y;
						}
					}
				}
				double degree = atan2(lineEd.y - lineSt.y, lineEd.x - lineSt.x) * (180.0/M_PI);
				mouseErr->x = correctLeftWallStPt.x - maxLPt.x;
				mouseErr->y = correctLeftWallStPt.y - maxLPt.y;
				mouseErr->degree = (int)(degree+0.5);
			}
			if(wall[2]==1)
			{
				if(rightWallCenter.x+20 < morphoImg.cols)
				{
					roiPt.x = rightWallCenter.x-20;
				}
				if(rightWallCenter.y-20 > 0)
				{
					roiPt.y = rightWallCenter.y-20;
				}
				cout << "wall7roi2" << endl;
				cv::Mat roi(morphoImg, cv::Rect(roiPt.x, roiPt.y, 40, 40));
				thinning(roi, thinImg);
				for(int y=0; y<thinImg.rows; y++)
				{
					for(int x=0; x<thinImg.cols; x++)
					{
						if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y < minY)
						{
							lineSt = cv::Point(x, y);
							minY = y;
						}
						if(thinImg.data[y*thinImg.step+x*thinImg.elemSize()] != 0 && y > maxY)
						{
							lineEd = cv::Point(x, y);
							maxY = y;
						}
					}
				}
				double degree = atan2(lineEd.y - lineSt.y, lineEd.x - lineSt.x) * (180.0/M_PI);
				mouseErr->x = correctRightWallStPt.x - maxRPt.x;
				mouseErr->y = correctRightWallStPt.y - maxRPt.y;
				mouseErr->degree = (int)(degree+0.5);
			}
		}
		// 
		else
		{
			if(wall[0]==1)
			{
			}
			if(wall[2]==1)
			{
			}
		}
	}
		cout << "est fin" << endl;
	//DEBUG
*/
/*
	cv::imshow("frontRoi",frontRoi);
	cv::imshow("leftRoi",leftRoi);
	cv::imshow("rightRoi",rightRoi);
	cout <<"front" << frontWallArea << frontWallCenter << endl;
	cout <<"left" << leftWallArea << leftWallCenter << endl;
	cout <<"right" << rightWallArea << rightWallCenter << endl;
	dstImg = cv::Mat(srcImg.size(), CV_8UC3);
	cv::Vec3b color;

	for(int y = 0; y < dstImg.rows; ++y)
	{
		for(int x = 0; x < dstImg.cols; ++x)
		{
			cv::Vec3b(morphoImg.at<int>(y, x), morphoImg.at<int>(y, x), morphoImg.at<int>(y, x));
		}
	}
*/	
/*
	//Labeling
	int nLabels = cv::connectedComponentsWithStats(morphoImg, labelImg, stats, centroids);
	std::vector<cv::Vec3b> colors(nLabels);
	colors[0] = cv::Vec3b(0, 0, 0);
	for(int label = 1; label < nLabels; ++label)
	{
		colors[label] = cv::Vec3b((rand()&255), (rand()&255), (rand()&255));
	}
	for(int y = 0; y < dstImg.rows; ++y)
	{
		for(int x = 0; x < dstImg.cols; ++x)
		{
			int label = labelImg.at<int>(y, x);
			cv::Vec3b &pixel = dstImg.at<cv::Vec3b>(y, x);
			pixel = colors[label];
		}
	}
	for(int label = 1; label < nLabels; ++label)
	{
		double *param = centroids.ptr<double>(label);
		int x = static_cast<int>(param[0]);
		int y = static_cast<int>(param[1]);
		cv::circle(dstImg, cv::Point(x,y), 3, cv::Scalar(0,0,255), -1);
	}
	

	if(wall[0] == 1){ cv::circle(dstImg, leftWallCenter, 3, cv::Scalar(255,255,255), -1);}
	if(wall[1] == 1){ cv::circle(dstImg, frontWallCenter, 3, cv::Scalar(255,255,255), -1);}
	if(wall[2] == 1){ cv::circle(dstImg, rightWallCenter, 3, cv::Scalar(255,255,255), -1);}

	// Thinning 
	thinning(morphoImg, thinImg);
*/
}
