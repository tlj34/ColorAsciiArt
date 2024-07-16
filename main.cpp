#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#define MOVE_CURSOR(x, y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {(x), (y)})
#define SET_COLOR(r, g, b) printf("\x1b[38;2;%d;%d;%dm", (r), (g), (b))
#define RESET_COLOR puts("\x1b[0m");
using namespace std;
using namespace cv;
int main() {
	Size outputSize = { 128, 40 };
	string charMap = " =*n3W@";
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo = { 1, 0 };
	SetConsoleCursorInfo(hOut, &cursorInfo);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	string videoName;
	getline(cin, videoName);
	VideoCapture video;
	video.open(videoName);
	int totalFrames = video.get(CAP_PROP_FRAME_COUNT);
	system("cls");
	cout << "Loading...";
	vector<vector<vector<Vec3b>>>color(totalFrames, vector<vector<Vec3b>>(outputSize.height, vector<Vec3b>(outputSize.width, { 0, 0, 0 })));
	vector<vector<vector<char>>>ch(totalFrames, vector<vector<char>>(outputSize.height, vector<char>(outputSize.width, 0)));
	for (int n = 0; n < totalFrames; ++n) {
		Mat tmp0, tmp1;
		video.read(tmp0);
		resize(tmp0, tmp0, outputSize, 0, 0);
		cvtColor(tmp0, tmp1, COLOR_BGR2GRAY);
		for (int i = 0; i < tmp0.rows; ++i)
			for (int j = 0; j < tmp0.cols; ++j) {
				color[n][i][j] = tmp0.at<Vec3b>(i, j);
				ch[n][i][j] = charMap[tmp1.at<uchar>(i, j) * charMap.size() / 256];
			}
	}
	system(videoName.c_str());
	Sleep(200);
	Vec3b last;
	clock_t currentTime = 0;
	clock_t totalTime = totalFrames / video.get(CAP_PROP_FPS) * CLOCKS_PER_SEC;
	clock_t startTime = clock();
	while (currentTime <= totalTime) {
		MOVE_CURSOR(0, 0);
		double current = 1.0 * currentTime / totalTime * totalFrames;
		for (int i = 0; i < outputSize.height; ++i) {
			for (int j = 0; j < outputSize.width; ++j) {
				Vec3b currentColor = color[(int)current][i][j];
				if (currentColor != last) {
					SET_COLOR(currentColor[2], currentColor[1], currentColor[0]);
					last = currentColor;
				}
				putchar(ch[(int)current][i][j]);
			}
			putchar('\n');
		}
		currentTime = clock() - startTime;
	}
	RESET_COLOR;
	return 0;
}