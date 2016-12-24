//cascadesTest.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <conio.h>
#include <fstream>

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2\core\ocl.hpp"


using namespace std;
using namespace cv;

void matrixTime();
bool callMethod(int choise);
void initSession();
bool createTestSamples();
bool checkCascade();
void detectAndDisplay(Mat frame, CascadeClassifier cascade);

int main()
{
	setlocale(LC_ALL, "Russian");
	initSession();
    return 0;
}


void matrixTime() {
	string matr("matrix");
	string buf;
	cin.clear();
	cin >> buf;
	int f = 0, i;
	i = 0;
	if (buf == matr) {
		char mas[6] = { '0','0' ,'0' ,'0' ,'0', '0' };
		system("cls");
		system("color 02");
		while (true) {
			cout << 10 + rand() % 90 << " ";
			if (_kbhit()) {
				if (i > 4) {
					for (int j = 0; j < 5; j++) {
						mas[j] = mas[j + 1];
					}
					i = 5;
				}
				mas[i] = _getch();
				i++;
			}
			if (mas[0] == matr.c_str()[0] &&
				mas[1] == matr.c_str()[1] &&
				mas[2] == matr.c_str()[2] &&
				mas[3] == matr.c_str()[3] &&
				mas[4] == matr.c_str()[4] &&
				mas[5] == matr.c_str()[5])
				break;
		}
		system("color 08");
		cout << "Bye!" << endl;
		system("cls");
	}
}


bool callMethod(int choise) {
	switch (choise) {
	case 0:
		return false;
		break;
	case 1:
		createTestSamples();
		break;
	case 2:
		checkCascade();
		break;
	default:
		return false;
		break;
	}
}

void initSession()
{
	const string ch = "Пожалуйста, сделайте выбор --> ";
	stringstream menu;
	menu << "0. Выход" << endl << "1. Задать тестовую выборку" << endl << "2. Проверить каскад" << endl;


	cout << menu.str().c_str() << ch.c_str();
	int choise = -1;
	cin >> choise;
	while (choise < 0 || choise > 5) {
		matrixTime();
		cout << "Вы ошиблись, пожалуйста, повторите --> ";
		cin >> choise;
	}
	while (callMethod(choise)) {
		system("pause");
		system("cls");
		cout << menu.str().c_str() << ch.c_str();
		cin >> choise;
		while (choise < 0 || choise > 5) {
			matrixTime();
			cout << "Вы ошиблись, пожалуйста, повторите --> ";
			cin >> choise;
		}
	}
}

CvCapture* capture = 0;

// функция-обработчик ползунка - 
// перематывает на нужный кадр
void myTrackbarCallback(int pos) {
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, pos);
}

bool createTestSamples() {
	ifstream info;
	info.open("test/info.txt");
	int curNum = 0;
	info >> curNum;
	info.close();


	cout << endl << endl << "Пожалуйста, введите путь к видео, кадры из которого нужно отобрать!" << endl;
	string fileName;
	cin >> fileName;
	string dest;

	cout << "Пожалуйста, введите интервал между кадрами в миллисекундах!" << endl;
	int interval;
	cin >> interval;
	capture = cvCreateFileCapture(fileName.c_str());

	// получаем число кадров
	double framesCount = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	printf("[i] count: %.0f\n", framesCount);
	int frames = (int)framesCount;

	int currentPosition = 0;
	if (frames != 0) {
		// показываем ползунок
		cvCreateTrackbar("Position", "original", &currentPosition, frames, myTrackbarCallback);
	}

	cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);

	int counter = curNum + 1;
	char imageFile[512];
	int i = 0;
	ofstream info1;
	info1.open("test/info.txt",ios::ate);


	while (i < framesCount) {
		// получаем следующий кадр
		i++;
		IplImage* frame = cvQueryFrame(capture);   //FRAME НЕ ДОЛЖЕН МОДИФИЦИРОВАТЬСЯ
		if (!frame) {
			break;
		}

		cvShowImage("capture", frame);

		char c = cvWaitKey(interval);
		if (c == 27) { // нажата ESC
			info1.clear();
			info1 << counter -1;
			cvDestroyAllWindows();
			break;
		}
		else if (c == 32) { // пробел - сохраняем кадр в файл
			sprintf(imageFile, "test/testSamples/Image%d.jpg", counter);
			cout << "Сделана фотография: " << imageFile << endl;
			cvSaveImage(imageFile, frame);
			info1.clear();
			info1 << counter;
			counter++;
		}
	}
	cvDestroyAllWindows();
	return true;
}

void detectAndDisplay(Mat frame, CascadeClassifier cascade)
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cv::ocl::setUseOpenCL(false);
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	
	Mat frame_gray2 = frame_gray;

	//-- Detect faces
	//cascade.detectMultiScale(frame_gray2, faces, 1.4, 2, 0, Size(40, 100), Size(135, 360));
	cascade.detectMultiScale(frame_gray2, faces, 1.4, 2, 0, Size(40, 100), Size(135, 360));
	
	

	for (size_t i = 0; i < faces.size(); i++)
	{
		Mat faceROI = frame_gray(faces[i]);
		std::vector<Rect> eyes;
		rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(0, 255, 0), 3, 8, 0);
		
	}
	imshow("capture", frame);

}

bool checkCascade() {
	cout << "Введите путь к каскаду!" << endl;
	string cascadeName;
	cin >> cascadeName;
	CascadeClassifier cascade;
	if (!cascade.load(cascadeName.c_str())) { printf("--(!)Error loading face cascade\n"); return false; };

	IplImage* image = 0;
	IplImage* clone = 0;

	ifstream info;
	info.open("test/info.txt");
	int curNum = 0;
	info >> curNum;
	info.close();

	cvNamedWindow("capture", CV_WINDOW_AUTOSIZE);
	Mat mat;
	//int j = 207;

	int aErrors = 0;
	int bErrors = 0;

	
	int aBuf = 0;
	int bBuf = 0;

	for (int i = 1; i <= curNum; i++) {
		aBuf = 0;
		bBuf = 0;
		system("cls");
		cout << cascadeName << endl;
		cout << "Ошибок первого рода всего: " << aErrors << endl;
		cout << "Ошибок второго рода всего: " << bErrors << endl;
		cout << "Ошибок первого рода на текущем кадре: " << aBuf << endl;
		cout << "Ошибок второго рода на текущем кадре: " << bBuf << endl;

		
		stringstream buf;
		buf << "test/testSamples/Image" << i << ".jpg";
		image = cvLoadImage(buf.str().c_str(), 1);
		clone = cvCloneImage(image);
		mat =cv::cvarrToMat(clone);
		
		detectAndDisplay(mat, cascade);


		while (true) {
			char c = cvWaitKey(2);
			if (c == 32) { // нажата ESC
				
				break;
			}
			/*if (c == 'g') {
				stringstream good;
				good << "test/goodSamples/Image" << j << ".jpg";
				j++;
				cvSaveImage(good.str().c_str(), image);
				break;
			}*/
			if (c == 'f') {
				aErrors++;
				aBuf++;
				system("cls");
				cout << cascadeName << endl;
				cout << "Ошибок первого рода всего: " << aErrors << endl;
				cout << "Ошибок второго рода всего: " << bErrors << endl;
				cout << "Ошибок первого рода на текущем кадре: " << aBuf << endl;
				cout << "Ошибок второго рода на текущем кадре: " << bBuf << endl;
			}
			if (c == 's') {
				bErrors++;
				bBuf++;
				system("cls");
				cout << cascadeName << endl;
				cout << "Ошибок первого рода всего: " << aErrors << endl;
				cout << "Ошибок второго рода всего: " << bErrors << endl;
				cout << "Ошибок первого рода на текущем кадре: " << aBuf << endl;
				cout << "Ошибок второго рода на текущем кадре: " << bBuf << endl;
			}
			if (c == 'c') {
				bErrors -= bBuf;
				aErrors -= aBuf;
				bBuf = 0;
				aBuf = 0;
				system("cls");
				cout << cascadeName << endl;
				cout << "Ошибок первого рода всего: " << aErrors << endl;
				cout << "Ошибок второго рода всего: " << bErrors << endl;
				cout << "Ошибок первого рода на текущем кадре: " << aBuf << endl;
				cout << "Ошибок второго рода на текущем кадре: " << bBuf << endl;
			}
		}
	}
	ofstream outFile;
	stringstream name;
	name << "test/";
	cout << "Пожалуйста, введите имя файла, в который нужно выгрузить результат!" << endl;
	string b;
	cin >> b;
	name << b << ".txt";
	outFile.open(name.str().c_str());
	outFile << "Число ошибок первого рода: " << aErrors << endl << "Число ошибок второго рода: " << bErrors << endl;
	outFile << "Среднее число ошибок первого рода на кадр: " << (float)aErrors / curNum << endl << "Среднее число ошибок второго рода на кадр: " << (float)bErrors / curNum << endl;
	cvDestroyAllWindows();
}

