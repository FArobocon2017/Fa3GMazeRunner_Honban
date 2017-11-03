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
	cv::Mat dstImg;
	cv::Mat hsvImg;
	cv::Mat maskImg;
	cv::Mat morphoImg;
	cv::Mat thinImg;

	// Labeling
	cv::Mat labelImg;
	cv::Mat stats;
	cv::Mat centroids;
	
	// Train Image
	cv::Mat trainLImg;
	cv::Mat trainTImg;
	cv::Mat trainCrossImg;

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
	cv::Point frontWallCenter, leftWallCenter, rightWallCenter;

	// Detection Parameter
	int rotNum;
	int rotAngle;
	double threshold;

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

public:
	RaspiCam();
	~RaspiCam();
	void open();
	void createWindow();
	void showImg();
	void writeImg();
	void createTrainImg(int pattern);
	void loadTrainImg();
	int detectCorner(cv::Mat inImg, cv::Mat trainImg, cv::Point& retPt);
	void getWallData(int *wall, EstimatedErrors *mouseErr);
};