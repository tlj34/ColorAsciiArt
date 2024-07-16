#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;
using namespace cv;

int main() {
	Size outputSize = { 256, 72 }; //�ַ����Ĵ�С��Ҫע�����̨ʹ�õĵȿ��ַ��İ���ַ���߱�һ����1:2����
	string charMap = " -=*n3W@"; //�ַ�����ʹ�õ��ַ��������ַ���ʱ����ݵ������ص�������������Ѱ�Ҷ�Ӧ���ַ�

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); //��ȡ���
	
	//���ع��
	CONSOLE_CURSOR_INFO cursorInfo = { 1, 0 };
	SetConsoleCursorInfo(hOut, &cursorInfo);
	
	//����Windows����̨�����ն�����
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	
	string videoName;
	cout << "��������Ƶ·����";
	getline(cin, videoName);
	VideoCapture video;
	video.open(videoName);
	
	//���������������洢��Ƶ��Ϣ���ַ���
	int totalFrames = video.get(CAP_PROP_FRAME_COUNT);
	vector<vector<vector<Vec3b>>>color(totalFrames, vector<vector<Vec3b>>(outputSize.height, vector<Vec3b>(outputSize.width, { 0, 0, 0 })));
	vector<vector<vector<char>>>ch(totalFrames, vector<vector<char>>(outputSize.height, vector<char>(outputSize.width, 0)));
	
	//�����ַ���
	cout << "Loading...";
	for (int n = 0; n < totalFrames; ++n) {
		Mat tmp0, tmp1;
		video.read(tmp0);
		resize(tmp0, tmp0, outputSize, 0, 0); //��ÿһ֡ԭͼ�Ĵ�С�ȱ���С�������С
		cvtColor(tmp0, tmp1, COLOR_BGR2GRAY);

		for (int i = 0; i < tmp0.rows; ++i)
			for (int j = 0; j < tmp0.cols; ++j) {
				color[n][i][j] = tmp0.at<Vec3b>(i, j); //��ȡÿһ֡��ÿ�����ص����ɫ
				ch[n][i][j] = charMap[tmp1.at<uchar>(i, j) * charMap.size() / 256]; //��ȡÿһ֡��ÿ�����ص�ת�����ַ�
			}
	}

	system("cls");
	system(videoName.c_str()); //��Ĭ�ϲ���������Ƶ
	Sleep(200); //��ʹ�õĲ���������Ƶʱ�����200ms���ӳ٣�������������Sleep��200ms
	
	Vec3b last = { 0, 0, 0 };
	clock_t currentTime = 0;
	clock_t totalTime = totalFrames / video.get(CAP_PROP_FPS) * CLOCKS_PER_SEC;
	clock_t startTime = clock();

	//�����ַ�����
	while (1) {
		//��ȡ��ǰ֡
		currentTime = clock() - startTime;
		int current = 1.0 * currentTime / totalTime * totalFrames;
		
		if (current >= totalFrames) break;

		string output; //���ｫ����������ȴ����ַ������������������������ٶ�
		output.reserve(outputSize.height * (outputSize.width + 1) * 12);
		
		SetConsoleCursorPosition(hOut, { 0, 0 }); //������ƶ�������̨�����Ͻ�
		
		for (int i = 0; i < outputSize.height; ++i) {
			for (int j = 0; j < outputSize.width; ++j) {
				Vec3b currentColor = color[current][i][j];
				if (currentColor != last) { //��ǰ��ɫ����һ������ɫ��ͬʱ���������ı���ɫ������ʹ���˿���̨�����ն������������ն���ɫ
					output += "\x1b[38;2;" + to_string(currentColor[2]) + ';' + to_string(currentColor[1]) + ';' + to_string(currentColor[0]) + 'm';
					last = currentColor;
				}
				output += ch[current][i][j];
			}
			output += '\n';
		}

		puts(output.c_str()); //��ӡ�ַ���
	}

	puts("\x1b[0m"); //��ԭ�ն���ɫ
	return 0;
}