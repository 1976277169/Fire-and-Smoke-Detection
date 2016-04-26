//-----------------------------------����----------------------------  
//     �������ã��ֿ�֡���˶����and������ɫģ��    
//     OpenCVԴ����汾��3.0.0    
//     by��xuejiguang
//     Time:2016/04/25	
//-------------------------------------------------------------------
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
using namespace std;
using namespace cv;

//�����ֵͼ���ش���0�ĸ���
int bSums(Mat src)
{
	int counter = 0;
	//�������������ص�
	Mat_<uchar>::iterator it = src.begin<uchar>();
	Mat_<uchar>::iterator itend = src.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)>0) counter += 1;//��ֵ�������ص���0����255
	}
	return counter;
}
//ͼ���ṹ��
struct FraRIO
{
	Mat frameRIO;
	int point_x;
	int point_y;
	bool RIO_flag;

};
//ÿ֡ͼ��ֿ�
vector<FraRIO>  DivFra(Mat &image, int width, int height)
{
	char name = 1;
	int m, n;
	m = image.rows / height;
	n = image.cols / width;
	vector<FraRIO> FraRIO_Out;
	FraRIO temFraRIO;
	for (int j = 0; j<m; j++)
	{
		for (int i = 0; i<n; i++)
		{
			Mat temImage(height, width, CV_8UC3, cv::Scalar(0, 0, 0));//
			Mat imageROI = image(Rect(i*width, j*height, temImage.cols, temImage.rows));//rect(x, y, width, height)ѡ������Ȥ����
			addWeighted(temImage, 1.0, imageROI, 1.0, 0., temImage);//����ɨ����ı߽�������

			temFraRIO.frameRIO = temImage.clone();
			temFraRIO.point_x = i*width;
			temFraRIO.point_y = j*height;
			FraRIO_Out.push_back(temFraRIO);
		}
	}
	return FraRIO_Out;
}

void ImgMean(float& c1, float& c2, float& c3, Mat pImg)
{
	int nPixel = pImg.rows*pImg.cols;	// number of pixels in image
	c1 = 0; c2 = 0; c3 = 0;

	//�ۼӸ�ͨ����ֵ
	MatConstIterator_<Vec3b> it = pImg.begin<Vec3b>();
	MatConstIterator_<Vec3b> itend = pImg.end<Vec3b>();

	while (it != itend)
	{
		c1 += (*it)[0];
		c2 += (*it)[1];
		c3 += (*it)[2];
		it++;

	}
	//�ۼӸ�ͨ����ֵ

	c1 = c1 / nPixel;
	c2 = c2 / nPixel;
	c3 = c3 / nPixel;
}

Mat ColorDet(Mat srcImg){
	Mat m_pcurFrameYCrCb;
	Mat pImgResult;

	m_pcurFrameYCrCb.create(srcImg.size(), srcImg.type());
	pImgResult.create(srcImg.size(), srcImg.type());
	//cvtColor(srcImg, m_pcurFrameYCrCb, CV_BGR2YCrCb);
	m_pcurFrameYCrCb = srcImg.clone();

	float yy_mean = 0, cr_mean = 0, cb_mean = 0;
	ImgMean(cb_mean, cr_mean, yy_mean, m_pcurFrameYCrCb);
	uchar r = 0, g = 0, b = 0;
	uchar yy = 0, cr = 0, cb = 0;


	for (int i = 0; i<srcImg.rows; i++){
		for (int j = 0; j<srcImg.cols; j++){

			b = srcImg.at<Vec3b>(i, j)[0];
			g = srcImg.at<Vec3b>(i, j)[1];
			r = srcImg.at<Vec3b>(i, j)[2];

			cb = m_pcurFrameYCrCb.at<Vec3b>(i, j)[0];
			cr = m_pcurFrameYCrCb.at<Vec3b>(i, j)[1];
			yy = m_pcurFrameYCrCb.at<Vec3b>(i, j)[2];

			if (r>120 && yy>cb&&cr>cb&&yy>yy_mean && (abs(cb - cr)>40))
				//if (r>12 && r>g && g>b && yy>cb&&cr>cb && cr>cr_mean  && cb<cb_mean && yy>yy_mean && (abs(cb - cr)>40))
			{
				pImgResult.at<Vec3b>(i, j)[0] = 255;
				pImgResult.at<Vec3b>(i, j)[1] = 255;
				pImgResult.at<Vec3b>(i, j)[2] = 255;

			}
			else
			{
				pImgResult.at<Vec3b>(i, j)[0] = 0;
				pImgResult.at<Vec3b>(i, j)[1] = 0;
				pImgResult.at<Vec3b>(i, j)[2] = 0;

			}

		}
	}
	//��ɫͼת�Ҷ�ͼ
	cvtColor(pImgResult, pImgResult, COLOR_BGR2GRAY);

	return pImgResult;
}

int main()
{
	//��ȡ��Ƶ
	VideoCapture capture("C:/Users/Administrator/Desktop/FireCollect/pos/fire indoor/fire_indoor_16.avi");
	if (!capture.isOpened())
		return -1;

	//
	double rate = capture.get(CV_CAP_PROP_FPS);
	int delay = 1000 / rate;

	//���嵱ǰ֡����ʱ֡
	Mat frame, tem_frame;

	//����ǰһ֡�͵�ǰ֡�ָ��Ľṹ������
	vector<FraRIO> framePro_RIO;
	vector<FraRIO> frame_RIO;

	bool flag = false;

	namedWindow("image", CV_WINDOW_AUTOSIZE);

	while (capture.read(frame)){
		//�Ե�ǰ֡���зָ��С24*24
		frame_RIO = DivFra(frame, 24, 24);

		if (false == flag)
		{
			//��ǰ֡���鸳ֵǰһ֡����
			framePro_RIO = frame_RIO;
			flag = true;
		}
		else
		{
			//
			vector<FraRIO>::iterator it_pro = framePro_RIO.begin();
			vector<FraRIO>::iterator it = frame_RIO.begin();
			//�Խṹ��������б���
			while (it != frame_RIO.end() && it_pro != framePro_RIO.end())
			{
				//��ǰ֡��ǰһ֡����,������ʱ֡
				absdiff(it->frameRIO, it_pro->frameRIO, tem_frame);
				//��ɫͼת�Ҷ�ͼ
				cvtColor(tem_frame, tem_frame, COLOR_BGR2GRAY);
				//��ֵ��
				threshold(tem_frame, tem_frame, 80, 255, CV_THRESH_BINARY);

				//cout << bSums(ColorDet(it->frameRIO)) << endl;
				tem_frame = tem_frame & ColorDet(it->frameRIO);
				//cout << bSums(tem_frame) << endl;

				//�ҳ����ش���0������
				if (bSums(tem_frame)>0){
					//�������ο�
					rectangle(frame, cvPoint(it->point_x, it->point_y), cvPoint(it->point_x + it->frameRIO.cols, it->point_y + it->frameRIO.rows), Scalar(255, 0, 0), 1, 1, 0);//�ܹ�ʵʱ��ʾ�˶�����
				}
				it++;
				it_pro++;
			}
			//
			framePro_RIO = frame_RIO;

			imshow("image", frame);

			waitKey(delay);
		}
	}
	return 0;
}