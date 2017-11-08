#include <raspicam/raspicam_cv.h>

struct EstimatedErrors
{
	int x;
	int y;
	int degree;
};

class RaspiCam
{
private:
	// Camera
	raspicam::RaspiCam_Cv Camera;

	// Image
	int imgWidth, imgHeight;
	cv::Mat srcImg;
	cv::Mat frontRoi;
	cv::Mat leftRoi;
	cv::Mat rightRoi;
	cv::Mat frontBinImg;
	cv::Mat leftBinImg;
	cv::Mat rightBinImg;
	cv::Mat dstImg;
	cv::Mat latestImg;

	// Image Processing Area
	int FBW;	// Front Border Width 
	int LBW;	// Left Border Width
	int RBW;	// Right Border Width 
	cv::Size frontProcArea;		
	cv::Size leftProcArea;
	cv::Size rightProcArea;
	cv::Rect frontProcRect;		
	cv::Rect leftProcRect;
	cv::Rect rightProcRect;
	int correctFrontWallArea;	
	int correctLeftWallArea;
	int correctRightWallArea;
	
	// Moment
	int frontWallArea, leftWallArea, rightWallArea;
	cv::Point frontWallCenterPt, leftWallCenterPt, rightWallCenterPt;
	cv::Point nearestFrontWallPt, nearestLeftWallPt, nearestRightWallPt;

	// Correct Wall Pt
	cv::Point correctFrontWallStPt;		// Left
	cv::Point correctFrontWallMidPt;	// Middle
	cv::Point correctFrontWallEdPt;		// Right

	cv::Point correctLeftWallStPt;		// High
	cv::Point correctLeftWallMidPt;		// Middle
	cv::Point correctLeftWallEdPt;		// Low

	cv::Point correctRightWallStPt;		// High
	cv::Point correctRightWallMidPt;	// Middle
	cv::Point correctRightWallEdPt;		// Low

	// Correct Mouse Pt
	cv::Point correctMouseCenterPt;
	cv::Point correctMouseFPt;
	cv::Point correctMouseFLPt;
	cv::Point correctMouseFRPt;
	
	// Correct Pillar Pt
	cv::Point leftPillarPt, rightPillarPt;
	cv::Size pillarSize;
	cv::Mat leftPillarRoi, rightPillarRoi;
	cv::Mat frontThinRoi;

public:
	RaspiCam();
	~RaspiCam();
	void open();
	void close();
	void capture();
	void createWindow();
	void showImg();
	void writeImg();
	int calcSideWallCenter(cv::Mat img, cv::Point& center);
	int calcFrontWallCenter(cv::Mat img, cv::Point& center);
	int calcNearestLeftWallPt(cv::Mat img, cv::Point& nearest);
	int calcNearestRightWallPt(cv::Mat img, cv::Point& nearest);
	int calcNearestFrontWallPt(cv::Mat img, cv::Point& nearest);
	void getFrontWallData(int *wall, EstimatedErrors *mouseErr);
	void getSideWallData(int *wall, EstimatedErrors *mouseErr);
};
