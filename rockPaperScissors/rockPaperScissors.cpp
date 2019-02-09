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
vector<vector<Point> > contourslineBuf;//для передачи из процедуры mline
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
	//vector<vector<Point> > contoursline;
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
			Point(lines[i][2], lines[i][3]),255, 30, 8);///рисование линий Scalar(255)
	}

	/////////плохо апроксмимровали
	/*resize(imagecan2,mimg, Size(imagecan2.cols / 8, imagecan2.rows / 8), INTER_CUBIC);
	resize(mimg,imagecan2, Size(imagecan2.cols, imagecan2.rows), INTER_CUBIC);*/
	//////////////////////////////
	////////////////////////////////////////////////
	contourslineBuf.clear();
	hierarchy.clear();
	findContours(imagecan2, contourslineBuf, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);//поиск контуров
	
	imagecan2 = 0;
	drawContours(imagecan2, contourslineBuf, -1, Scalar(255), 3);//CV_FILLED рисование контуров
	imshow("lineCont",imagecan2);
	/////////////////////////////////////////////////////////
	return imagecan2;
}
static void savekon(Mat grey,const String name , vector<vector<Point> > contoursline)
{
	Mat imgk;
	vector<vector<Point> > contoursvibor;//хранилище выбранного
	
	bool qe=0;//переменная для выхода
	bool ve;//флаг продолжение
	grey.copyTo(imgk);
	ofstream ofs("k1.txt");
	FileStorage fs(name, FileStorage::WRITE);
	


	//fs << "cont" << contoursline;//все записал
	Moments m;//моменты
	

	double hm7[7];
	
	char c;
	for (;;)
	{
		for (size_t k = 0; k < contoursline.size(); k++)//перебор контуров
		{   
			ve = true;//на месте
			
			do
			{
				imgk = 0;
				drawContours(imgk, contoursline, k, Scalar(255), 3);
				imshow("saveK", imgk);
				c = waitKey(100);
				switch (c)
				{
					case 's':
					ofs << contoursline[k];//в текстовый файл
					///HuMoments
					contoursvibor.push_back(contoursline[k]);//хранилище выбранного
					m = moments(contoursline[k]);//получение моментов
					HuMoments(m, hm7);//получение нормализованного моментов
					fs << "contVibor" << contoursvibor;//все записал
													   //fs << "Moments" << m.;
					fs << "HuMoments0" << hm7[0];//все записал
					fs << "HuMoments1" << hm7[1];
					fs << "HuMoments2" << hm7[2];
					fs << "HuMoments3" << hm7[3];
					fs << "HuMoments4" << hm7[4];
					fs << "HuMoments5" << hm7[5];
					fs << "HuMoments6" << hm7[6];
					cout << "button s ";
					break;
					case 'q'://проверка выход
					ve = false;
					qe = true;
					cout << "button q ";
					break;
					case 'v'://дальше
						cout << "button v";
					ve = false;
					break;
				}
			} while (ve);
			if (qe == true) break;//выход
		}
		//break;
		if (qe == true) break;//выход
	}
	
	fs.release();
	
	destroyWindow("saveK");//закрыть окно
}
static void loadkon(Mat grey)
{
	FileStorage fs("k1.xml", FileStorage::READ);//из библиотеки opencv
	if (fs.isOpened() == false) return ;//выход из процедуры
	vector<Point> contur;
	Mat m;
	FileNode nb = fs["cont"];
	FileNodeIterator it = nb.begin(), it_end = nb.end(); // создание итератора


	for (; it != it_end; ++it)
	{
		

		(*it) >> contur;//запись в контур
		
		

	}

	fs.release();
	grey.copyTo(m);
	m = 0;
	drawContours(m,contur, -1, Scalar(255), 3);
	imshow("LoadK",m);
}
static void findknb(Mat grey, vector<vector<Point> > contoursline) //поиск сравнение контуров
{
	bool qe = 0;//переменная для выхода
	bool ve;//флаг продолжение
	Mat imgk;
	char c;
	grey.copyTo(imgk);//иначе будет ошибка
	FileStorage fsk("kamen.xml", FileStorage::READ);//из библиотеки opencv
	FileStorage fsb("bum.xml", FileStorage::READ);//из библиотеки opencv
	FileStorage fsn("nozh.xml", FileStorage::READ);//из библиотеки opencv
	vector<vector<Point> > conturk;
	vector<vector<Point> > conturn;
	vector<vector<Point> > conturb;
	vector<vector<Point> > contoursvibor;
	FileNode nk = fsk["contVibor"];
	
	FileNodeIterator it = nk.begin(), it_end = nk.end(); // создание итератора
	
	for (; it != it_end; ++it)
	{
		vector<Point> tmp;//контур
		//cout << (int)*it << endl;
		
		(*it) >> tmp;//запись в контур
		cout << tmp << endl;
		conturk.push_back(tmp);
		
	}
	
	cout << "  ";
	
	FileNode nb = fsb["contVibor"] ;
	FileNodeIterator itb = nb.begin(), itb_end = nb.end(); // создание итератора

	for (; itb != itb_end; ++itb)
	{
		vector<Point> tmpb;//контур
						  //cout << (int)*it << endl;

		(*itb) >> tmpb;//запись в контур
		cout << tmpb << endl;
		conturb.push_back(tmpb);

	}
	cout << "  ";
	FileNode nn = fsn["contVibor"] ;
	FileNodeIterator itn = nn.begin(), itn_end = nn.end(); // создание итератора

	for (; itn != itn_end; ++itn)
	{
		vector<Point> tmpn;//контур
						   //cout << (int)*it << endl;

		(*itn) >> tmpn;//запись в контур
		cout << tmpn << endl;
		conturn.push_back(tmpn);

	}

	cout << "  ";
	
	namedWindow("Find", WINDOW_AUTOSIZE);
	//matchShapes();//сравнение 
	for (;;)
	{
		for (size_t k = 0; k < contoursline.size(); k++)//перебор контуров
		{
			ve = true;//на месте

			do
			{
				imgk = 0;
				drawContours(imgk, contoursline, k, Scalar(255), 3);
				try{ 
					imshow("Find", imgk); 
				}
				catch (exception ex)
				{
					cout << "Oshibka ";
					

				}
				c = waitKey(100);
				double km, nzh, bm;//значения сравнений
				switch (c)
				{
					case 'd'://данные
						//contoursvibor.push_back(contoursline[k]);
						km = matchShapes(contoursline[k], conturk[0],1,0);//хранилище выбранного)
						cout << "button d ";
						nzh = matchShapes(contoursline[k], conturn[0], 1, 0);
						bm = matchShapes(contoursline[k], conturb[0], 1, 0);
						cout << " km ";
						cout << km;
						cout << "    ";
						cout << " nzh ";
						cout << nzh;
						cout << "    ";
						cout << " bm ";
						cout << bm;
						cout << "    ";
					break;
					case 'q'://проверка выход
						ve = false;
						qe = true;
						cout << "button q ";
					break;
					case 'v'://дальше
						cout << "button v";
						ve = false;
						break;
				}
			} while (ve);
			if (qe == true) break;//выход

		}
		if (qe == true) break;//выход
	}

	destroyWindow("Find");//закрыть окно
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
			
			
			case 'c'://выводить канни или контуры после нажатия меняет
			
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
			case 's'://после записи линий нажимать иначе ошибка записьлюбого контура
				if (fcanny == 3) savekon(grey,"k1.xml", contourslineBuf);
				if (fcanny == 1) savekon(grey, "k1.xml",contours);
				if (fcanny == 2) savekon(grey, "k1.xml", contours);
				break;
			case 'k'://после записи линий нажимать иначе ошибка запись контура камня
				if (fcanny == 3) savekon(grey, "kamen.xml",contourslineBuf);
				if (fcanny == 1) savekon(grey, "kamen.xml", contours);
				if (fcanny == 2) savekon(grey, "kamen.xml", contours);
				break;
			case 'n'://после записи линий нажимать иначе ошибка запись контура ножниц
				if (fcanny == 3) savekon(grey, "nozh.xml", contourslineBuf);
				if (fcanny == 1) savekon(grey, "nozh.xml", contours);
				if (fcanny == 2) savekon(grey, "nozh.xml", contours);
				break;
			case 'b'://после записи линий нажимать иначе ошибка запись контура бумаги
				if (fcanny == 3) savekon(grey, "bum.xml", contourslineBuf);
				if (fcanny == 1) savekon(grey, "bum.xml", contours);
				if (fcanny == 2) savekon(grey, "bum.xml", contours);
				break;

			case 'e'://сравнение контуров с шаблоном
				if (fcanny == 3) findknb(grey, contourslineBuf);
				if (fcanny == 1) findknb(grey, contours);
				if (fcanny == 2) findknb(grey, contours);
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

