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
	cv::Mat leftRoi;
	cv::Mat rightRoi;
	cv::Mat leftBinImg;
	cv::Mat rightBinImg;
	cv::Mat dstImg;

	// Image Processing Area
	int FBW;	// Front Border Width (not Use)
	int LBW;	// Left Border Width
	int RBW;	// Right Border Width 
	cv::Size frontProcArea;		// not Use
	cv::Size leftProcArea;
	cv::Size rightProcArea;
	cv::Rect frontProcRect;		// not Use
	cv::Rect leftProcRect;
	cv::Rect rightProcRect;
	int correctFrontWallArea;	// not Use
	int correctLeftWallArea;
	int correctRightWallArea;
	
	// Moment
	int frontWallArea, leftWallArea, rightWallArea;
	cv::Point frontWallCenter, leftWallCenter, rightWallCenter;


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
	cv::Point correctMousePt;
	
	// Correct Pillar Pt
	cv::Point leftPillarPt, rightPillarPt;
	cv::Size pillarSize;
	cv::Mat leftPillarRoi, rightPillarRoi;
	
public:
	RaspiCam();
	~RaspiCam();
	void open();
	void close();
	void createWindow();
	void showImg();
	void writeImg();
	void getWallData(int *wall, EstimatedErrors *mouseErr);
};
