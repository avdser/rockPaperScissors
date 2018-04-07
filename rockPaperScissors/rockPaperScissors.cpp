// rockPaperScissors.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <fstream> 
using namespace cv;
using namespace std;
const int w = 500;
int levels = 3;
vector<vector<Point> > contours0;
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
vector<Moments> mu;
Mat hu;
int v1=30;
double hhu[7];
static void on_trackbar(int, void*)
{
	Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
	int _levels = levels - 3;
	drawContours(cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128, 255, 255),
		3, LINE_AA, hierarchy, std::abs(_levels));

	imshow("contours", cnt_img);
}
int main()
{
	ofstream ofs("date.txt");
	
	Mat image,image3,imageL,imageCont;
	Mat imageHSV;
	Mat grey,grey2;
	Mat fon,imagef;
	char c;//ключ нажатия 
	bool f=false;//ключ вычитания
	VideoCapture capture;
	int element_shape = MORPH_RECT;
	Mat element = getStructuringElement(element_shape, Size(v1,100), Point(-1, -1));//получение ядра
	InputArray low_red = (0, 0, 0);//минимум (188, 143, 143)
	InputArray high_red = (255, 255, 255);//максимум (139, 69, 19)
	capture.open(0);
	cout <<Scalar(130, 130, 130);
	Mat cnt_img;
	
	if (capture.isOpened())
	{
		cout << "Capture is opened" << endl;
		for (;;)
		{
			capture >> image3;
			image3 = image3 * 0.25;//подбор яркости
			//resize(image3, imageL, Size(image3.cols/8,image3.rows/8));
			//resize(imageL, image3, Size(image3.cols, image3.rows));
			medianBlur(image3,image,15);//устранение помех
			//erode(image, image3, element);////
			//dilate(image3, image, element);////устранение помех
			cvtColor(image, imageHSV, COLOR_BGR2HSV);
			if (f == true)
			{
				imagef = fon-image;//вычитание фона
				imshow("Разница", imagef);
			}
			
			inRange(imageHSV,Scalar(0,20,0),Scalar(20,255,255), grey);//выделение области по цвету  бинаризация inRange(imageHSV,Scalar(0,48,80),Scalar(20,255,255), grey)
			if (imageHSV.empty())
				break;
			//morphologyEx();
			//erode(grey2, grey, element);
			dilate(grey,grey2,element);//укрупнение
			erode(grey2, grey, element);
			//Sobel(grey2, grey, CV_8U,1,1,3);//контуры
			//imshow("SampleHSV", imageHSV);
			contours0.clear();//очистка
			hierarchy.clear();//очистка
			contours.clear();
			mu.clear();
			hu=0;
			///////////////////////////////////////////////////////////////////
			findContours(grey, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);//поиск контуров
			//contourArea(contours0);//площадь контура 
			contours.resize(contours0.size());
			mu.resize(contours0.size());
			hu.resize(contours0.size()*7);//размер
			
			
			for (size_t k = 0; k < contours0.size(); k++)
			{
				approxPolyDP(Mat(contours0[k]), contours[k], 3, true);
				/*mu[k] = moments(contours[k],false);
				HuMoments(mu[k], hhu);
				**hu[k] = *hhu;*///моменты
				///matchShapes()///сравнение контуров
					
			}
			
			grey.copyTo(imageCont);
			//imageCont(int(grey.rows),int(grey.cols),grey.type());неправильно
			imageCont = 0;
			drawContours(imageCont, contours, -1, Scalar(255, 255, 255), 3);//CV_FILLED
			//namedWindow("contours", 1);
			imshow("contours",imageCont);
			/*createTrackbar("levels+3", "contours", &levels, 7, on_trackbar);

			on_trackbar(0, 0);*/
			//////////////////////////////////////////////////////////////////////
			//addWeighted(image,0.5,grey,0.5,0.5,image3);
			
			for (int r = 0; r < imageCont.rows;r++)// r < image.rows;r++
			{
				for (int c=0; c < imageCont.cols;c++)//  c < image.cols
				{
					try
					{
						if (imageCont.at<char>(r, c) != 0 )
						{
							image3.at<Vec3b>(r, c)[0] = 128;
							image3.at<Vec3b>(r, c)[1] = 255;
							image3.at<Vec3b>(r, c)[2] = 255;
						}
					}
					catch (exception ex)
					{
						cout << "Oshibka " ;
						cout << " r " << r;
						cout << " c " << c<<" "<<grey.at<Vec3b>(r, c)[0]<<"  "<< grey.at<Vec3b>(r, c)[1]<<" "<< grey.at<Vec3b>(r, c)[2]<<"  ";
						system("pause");//пауза 
						
					}

				}

			}
			imshow("Sample", image3);
			imshow("Grey", grey);
			//imshow("Sobel",grey);
			
			c=waitKey(100);
			if (c=='f')//запись фона
			{
				image.copyTo(fon);
				f = true;
			}
			if (c == 'v')
			{
				cout << "Input v1: ";
				cin >> v1;//ввод значения
				system("pause");
			}
			if (c =='q' || c == 27)//waitKey(10) >= 0)
			{
				ofs << image;
				//cout << image;
				//cout<<image.rows;
				//cout<<image.cols;
				//ofs<<image.at<char>(1,2);
				

				ofs.close();
				//system("pause");//пауза 
				break;
			}
				
		}
	}
    return 0;
}

