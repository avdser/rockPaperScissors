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
vector<vector<Point> > contoursline;
vector<Vec4i> hierarchy;
vector<Moments> mu;
Mat image3;
Mat hu;
Mat image, imageL, imageM, imageCont;
Rect select;//область выделения
int v1=30;
double hhu[7];
/*static void on_trackbar(int, void*)
{
	Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
	int _levels = levels - 3;
	drawContours(cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128, 255, 255),
		3, LINE_AA, hierarchy, std::abs(_levels));

	imshow("contours", cnt_img);
}*/
static void on_trackbar(int, void*)//для ползунка
{
	imshow("MAIN", image3);
}
static void onMouse(int event, int x, int y, int, void*)
{
	if (event != EVENT_LBUTTONDOWN)
		return;
	int r, g, b;
	r = imageM.at<Vec3b>(y, x)[0];
	g = imageM.at<Vec3b>(y, x)[1];
	b = imageM.at<Vec3b>(y, x)[2];
	cout<< r;
	cout << " ";
	cout << g;
	cout << " ";
	cout << b;
}
static Mat mavg(Mat m)//заполнение матрицы средним
{
	int r, g, b;
	int k=0;//количество
	r = 0;
	g = 0;
	b = 0;
	for (int i = 0; i < m.rows; i++)
		for (int j = 0; j < m.cols; j++)
		{
			k = k +1 ;
			r=m.at<Vec3b>(i, j)[0]+r;
			g=m.at<Vec3b>(i, j)[1]+g;
			b=m.at<Vec3b>(i, j)[2]+b;
		}
	r = r / k;
	g = g / k;
	b = b / k;
	for (int i = 0; i < m.rows; i++)
		for (int j = 0; j < m.cols; j++)
		{
			m.at<Vec3b>(i, j)[0]=r;
			m.at<Vec3b>(i, j)[1]=g;
			m.at<Vec3b>(i, j)[2]=b;
		}
	return m;
}
static void kvadrat(int nr,Mat imgIsh)///вывожу квадрты
{
	int xr = 0, yr = 0, wr, hr;//координаты начальной точки rect select nr-число регионов
	Mat img;
	///ширина и высота квадрата
	imgIsh.copyTo(img);
	wr = abs(img.cols / nr);
	hr = abs(img.rows / nr);
	for (int iy = 1;iy <= nr;iy++)
	{
		for (int ix = 1;ix <= nr;ix++)
		{
			Mat img2;
			select = Rect(xr, yr, wr, hr);
			img(select).copyTo(img2);
			mavg(img2).copyTo(img(select));//вычисление среднего
			xr = xr + wr;
			
			
		}
		yr = yr + hr;
		xr = 0;
	}

	imshow("Регион", img);

}
static Mat kontur(Mat grey)///контуры
{
	vector<vector<Point> > contours1, contours2;
	vector<Vec4i> hierarchy2;
	Mat imagecan2;
	findContours(grey, contours1, hierarchy2, RETR_TREE, CHAIN_APPROX_SIMPLE);//поиск контуров
	contours2.resize(contours1.size());
	grey.copyTo(imagecan2);
	imagecan2 = 0;
	if(contours2.size() > 0)
	{
		for (size_t k = 0; k < contours0.size(); k++)//перебор контуров
		{

			//approxPolyDP(Mat(contours1[k]), contours2[k], 3, true);//апроксимация контура
			try {
				approxPolyDP(Mat(contours1[k]), contours2[k], 3, true);//апроксимация контура
			}
			catch (exception ex)
			{
				cout << " Oshibka ";
				cout << k;
				cout << "  ";
			}
				drawContours(imagecan2, contours2, k, Scalar(255), 3);//CV_FILLED рисование контуров
			
			
		}
	}
	return imagecan2;
}
static Mat mlines(Mat grey)///линии находим
{
	//vector<Vec2f> lines;
	vector<Vec4i> lines;
	Mat imagecan2,mimg;
	grey.copyTo(imagecan2);
	imagecan2 = 0;
	/*HoughLines(grey, lines, 1, CV_PI / 180, 100);
	for (size_t i = 0; i < lines.size(); i++)//рисуем линии
	{
		float rho = lines[i][0];
		float theta = lines[i][1];
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		Point pt1(cvRound(x0 + 1000 * (-b)),
			cvRound(y0 + 1000 * (a)));
		Point pt2(cvRound(x0 - 1000 * (-b)),
			cvRound(y0 - 1000 * (a)));
		line(imagecan2, pt1, pt2, Scalar(0, 0, 255), 3, 8);
	}*/
	HoughLinesP(grey, lines, 1, CV_PI / 180, 2, 1, 2);//сделал меньше порог срабатывания
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(imagecan2, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]),255, 50, 8);///рисование линий Scalar(255)
	}

	/////////плохо апроксмимровали
	/*resize(imagecan2,mimg, Size(imagecan2.cols / 8, imagecan2.rows / 8), INTER_CUBIC);
	resize(mimg,imagecan2, Size(imagecan2.cols, imagecan2.rows), INTER_CUBIC);*/
	//////////////////////////////
	////////////////////////////////////////////////
	contoursline.clear();
	hierarchy.clear();
	findContours(imagecan2, contoursline, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);//поиск контуров
	
	imagecan2 = 0;
	drawContours(imagecan2, contoursline, -1, Scalar(255), 3);//CV_FILLED рисование контуров
	/////////////////////////////////////////////////////////
	return imagecan2;
}
static void savekon(Mat grey)
{
	Mat imgk;
	bool qe=0;//переменная для выхода
	grey.copyTo(imgk);
	ofstream ofs("k1.txt");
	FileStorage fs("k1.xml", FileStorage::WRITE);
	fs << "cont" << contoursline;
	fs.release();
	char c;
	for (;;)
	{
		for (size_t k = 0; k < contoursline.size(); k++)//перебор контуров
		{
			imgk=0;
			drawContours(imgk, contoursline, k, Scalar(255), 3);
			imshow("saveK", imgk);
			c = waitKey(100);
			switch (c)
			{
			case 's':
				ofs << contoursline[k];
				break;
			case 'q':
				destroyWindow("saveK");//закрыть окно
				qe = true;
				
				break;
			}
		}
		if (qe == true) break;//выход
	}
}
static void loadkon(Mat grey)
{
	FileStorage fs("k1.xml", FileStorage::READ);
	vector<vector<Point> > contur;
	Mat m;
	fs["cont"] >> contur;
	fs.release();
	grey.copyTo(m);
	m = 0;
	drawContours(m,contur, -1, Scalar(255), 3);
	imshow("LoadK",m);
}
int main()
{
	ofstream ofs("date.txt");
	
	
	Mat imageHSV,imagecan,imagecan2;
	Mat grey,grey2;
	Mat fon,imagef;
	Mat imgreg,imgavg,imgavg1;
	int fcanny=1;
	int val=100;//значение ползунка
	int mincolor = 0;//значение ползунка
	int maxcolor = 20;//значение ползунка
	int nr=4;
	Scalar col[5] = {Scalar(10,255,30),Scalar(255,10,50),Scalar(255,200,50),Scalar(100,10,200),Scalar(50,100,50) };
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
	namedWindow("MAIN", WINDOW_AUTOSIZE);
	char TrackbarName[50]="val";
	//sprintf(TrackbarName, "val",1);
	select = Rect(0, 0,200,200 );
	createTrackbar(TrackbarName, "MAIN", &val, 200, on_trackbar);
	createTrackbar("mincolor", "MAIN", &mincolor, 255, on_trackbar);
	createTrackbar("maxcolor", "MAIN", &maxcolor, 255, on_trackbar);
	createTrackbar("NR", "MAIN", &nr, 10, on_trackbar);
	if (capture.isOpened())
	{
		cout << "Capture is opened" << endl;
		for (;;)///цикл 
		{
			capture >> image3;
			//kvadrat(nr, image3);//вывожу квадратами в окно Регион
			image3(select).copyTo(imgreg);
			/*reduce(imgreg, imgavg,0, REDUCE_AVG);
			reduce(imgavg, imgavg1, 1, REDUCE_AVG);*///попытка подсчитать средние
			//mavg(imgreg).copyTo(imgreg);//заполняю средним матрицу
			
			image3 = image3 * val*0.01;//подбор яркости * 0.25
			resize(image3, imageL, Size(image3.cols/64,image3.rows/64), INTER_CUBIC);
			resize(imageL, imageM, Size(image3.cols, image3.rows), INTER_CUBIC);
			imshow("Resize", imageM);
			///Canny
			cvtColor(image3, imagecan, COLOR_BGR2GRAY);
			Canny(imagecan,imagecan,50,200, 3);
			
			
			switch (fcanny)
			{
				case 2:
					imshow("Canny", kontur(imagecan));
					cout << " Kontur ";
					break;

				case 1:

					imshow("Canny", imagecan);
					cout << " Canny  ";
					break;
				case 3:

					imshow("Canny", mlines(imagecan));
					cout << "Lines ";
					break;

			}
			////
			setMouseCallback("Resize",onMouse,0 );//получение цвета по нажатия мышки
			//imageM.copyTo(image3);//для вывода
			//resize(imageL, image3, Size(image3.cols, image3.rows));
			medianBlur(image3,image,15);//устранение помех
			//?MeanShift(image, ilabels);
			pyrMeanShiftFiltering(image,image3,1,1);//сегментация по цвету делает переходы по цвету более четкие
			image3.copyTo(image);
			//erode(image, image3, element);////
			//dilate(image3, image, element);////устранение помех
			cvtColor(image, imageHSV, COLOR_BGR2HSV);
			if (f == true)
			{
				imagef = fon-image;//вычитание фона
				imshow("Разница", imagef);
			}
			
			inRange(imageHSV, Scalar(mincolor, 48, 80), Scalar(maxcolor, 255, 255), grey);//выделение области по цвету  бинаризация inRange(imageHSV,Scalar(0,48,80),Scalar(20,255,255), grey) inRange(imageHSV,Scalar(0,20,0),Scalar(20,255,255), grey)
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
			
			
			for (size_t k = 0; k < contours0.size(); k++)//перебор контуров
			{
				approxPolyDP(Mat(contours0[k]), contours[k], 3, true);//апроксимация контура
				/*mu[k] = moments(contours[k],false);
				HuMoments(mu[k], hhu);
				**hu[k] = *hhu;*///моменты
				///matchShapes()///сравнение контуров
				drawContours(image3, contours, k, col[k], 3);
					
			}
			
			grey.copyTo(imageCont);//создание имиджа для хранения контуров
			loadkon(grey);
			//imageCont(int(grey.rows),int(grey.cols),grey.type());неправильно
			imageCont = 0;//обнуление
			drawContours(imageCont, contours, -1, Scalar(255, 255, 255), 3);//CV_FILLED рисование контуров
			//namedWindow("contours", 1);
			imshow("contours",imageCont);//вывод на экран контуров
			/*createTrackbar("levels+3", "contours", &levels, 7, on_trackbar);

			on_trackbar(0, 0);*/
			//////////////////////////////////////////////////////////////////////
			//addWeighted(image,0.5,grey,0.5,0.5,image3);
			
			/*for (int r = 0; r < imageCont.rows;r++)// r < image.rows;r++//вывод контура на image3
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

			}*/
			imshow("MAIN", image3);
			imshow("Grey", grey);
			//imshow("Sobel",grey);
			//////////////////////////////////////////опрос кнопки
			c=waitKey(100);
			switch (c)
			{
			case 'f'://запись фона
				
					image.copyTo(fon);
					f = true;
					break;
			case 'v':
			
					cout << "Input v1: ";
					cin >> v1;//ввод значения
					system("pause");
					break;
			
			
			case 'c'://выводить канни или контуры
			
					switch (fcanny)
					{
					case 1:
						fcanny = 2;
						break;
					case 2:
						fcanny = 1;
						break;
					case 3:
						fcanny = 1;
						break;
					}
					if (fcanny == 1) kontur(grey);
					break;
			case	'l'://выводить канни или линии
				
					switch (fcanny)
					{
					case 1:
						fcanny = 3;
						break;
					case 2:
						fcanny = 3;
						break;
					case 3:
						
						fcanny = 1;
						break;

					}
					if (fcanny==3) mlines(grey);
					break;
			case 's'://после записи линий нажимать иначе ошибка
				savekon(grey);
				break;
			}
			if  (c=='q' ||c== 27)//waitKey(10) >= 0)
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

