#include "openCV.h"
#include "CvKinect.h"
void OpenCV::popupcv()
{
	cvSetImageROI(KinectColorImg, cvRect(0, 0, 640, 480));
	cvAddS(KinectColorImg, cvScalar(20), KinectColorImg);
	cvResetImageROI(KinectColorImg);
	cvNamedWindow("RoiAdd", 1);
	cvShowImage("OpenCV_Kinect!", KinectColorImg);
}
void Kinect::testcv()
{
	cvSmooth(KinectColorImg, KinectColorImg, CV_GAUSSIAN, 5, 5);
}
void Kinect::camshiftarr()
{
	
	//cvSetMouseCallback("CamShiftDemo", on_mouse, 0);
	cvCreateTrackbar("Vmin", "CamShiftDemo", &vmin, 256, 0);
	cvCreateTrackbar("Vmax", "CamShiftDemo", &vmax, 256, 0);
	cvCreateTrackbar("Smin", "CamShiftDemo", &smin, 256, 0);

	
	//if (!KinectColorImg)
	//{
		hsv = cvCreateImage(cvGetSize(KinectColorImg), 8, 3);
		hue = cvCreateImage(cvGetSize(KinectColorImg), 8, 1);
		mask = cvCreateImage(cvGetSize(KinectColorImg), 8, 1);
		backproject = cvCreateImage(cvGetSize(KinectColorImg), 8, 1);
		//������׷� ���� (ä��,size ,ǥ�����,x�����,���밣��
		hist = cvCreateHist(1, &hdims, CV_HIST_ARRAY, &hranges, 1);
		//������ �׷� ����� ���� ���� �� �ʱ�ȭ
		histimg = cvCreateImage(cvSize(320, 200), 8, 3);
		cvZero(histimg);
	//}
	//
	//cvCvtColor(KinectColorImg, hsv, CV_BGR2HSV);
	////
		
			if (!hsv)
			{

				hsv = cvCreateImage(cvGetSize(KinectColorImg), 8, 3);
				hue = cvCreateImage(cvGetSize(KinectColorImg), 8, 1);
				mask = cvCreateImage(cvGetSize(KinectColorImg), 8, 1);
				backproject = cvCreateImage(cvGetSize(KinectColorImg), 8, 1);
				//������׷� ���� (ä��,size ,ǥ�����,x�����,���밣��
				hist = cvCreateHist(1, &hdims, CV_HIST_ARRAY, &hranges, 1);
				//������ �׷� ����� ���� ���� �� �ʱ�ȭ
				histimg = cvCreateImage(cvSize(320, 200), 8, 3);
				cvZero(histimg);
			}
			if (trackObject)
			{

			}
		
}
CvScalar Kinect::hsv2rgb(float hue)
{
	int rgb[3], p, sector;
	static const int sector_data[][3] =
	{ { 0, 2, 1 }, { 1, 2, 0 }, { 1, 0, 2 }, { 2, 0, 1 }, { 0, 1, 2 } };
	hue *= 0.0333333333333f;
	sector = cvFloor(hue);//hue ���� �����Ͽ� ���������� ��ȯ
	p = cvRound(255 * (hue - sector));
	/*(Ȧ��&1)=1 (¦��&1)=0 sector�� Ȧ���� : sector&1 =1 ==>p=255 */
	p^=sector &1 ?255:0;//secotr�� ¦����:(sector&1=0==>p=0
	rgb[sector_data[sector][0]]=255;
	rgb[sector_data[sector][1]]=0;
	rgb[sector_data[sector][2]]=p;
	return cvScalar(rgb[2],rgb[1],rgb[0],0);
}
void Kinect::on_mouse(int event, int x, int y, int flags, void* param)
{
	if (!KinectColorImg)
		return;
	if (KinectColorImg->origin)
	{
		y = KinectColorImg->height - y;
	}
	//���콺 Ŭ�� �� ������ �̵��� ���� ��
	if (select_object)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = selection.x + CV_IABS(x - origin.x);
		selection.height = selection.y + CV_IABS(y - origin.y);

		selection.width = MIN(selection.width, KinectColorImg->width);
		selection.height = MIN(selection.height, KinectColorImg->height);
		selection.width -= selection.x;
		selection.height -= selection.y;
	}
	switch (event)
	{
		//���콺 ���� ��ư Ŭ��
	case CV_EVENT_LBUTTONDOWN:
		origin = cvPoint(x, y);
		selection = cvRect(x, y, 0, 0);
		select_object = 1;
		break;
		//���콺 ���ʹ�ư Ŭ���� release
	case CV_EVENT_LBUTTONUP:
		select_object = 0;
		//ROI �����Ǿ�����
		if (selection.width > 0 && selection.height > 0)
		{
			trackObject = -1;
		}
		break;
	}
}
//static void OpenCV::onMouse(int event, int x, int y, int, void *)
//{
//	if (selectObject)
//	{
//		selection.x = MIN(x, origin.x);
//		selection.y = MIN(y, origin.y);
//		selection.width = std::abs(x - origin.x);
//		selection.height = std::abs(y - origin.y);
//
//		selection &= Rect(0, 0, image.cols, image.rows);
//	}
//
//	switch (event)
//	{
//	case CV_EVENT_LBUTTONDOWN:
//		origin = Point(x, y);
//		selection = Rect(x, y, 0, 0);
//		selectObject = true;
//		break;
//	case CV_EVENT_LBUTTONUP:
//		selectObject = false;
//		if (selection.width > 0 && selection.height > 0)
//			trackObject = -1;
//		break;
//	}
//}