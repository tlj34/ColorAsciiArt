#include <opencv2/opencv.hpp>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;
using namespace cv;

int main() {
	Size outputSize = { 256, 72 }; //字符画的大小，要注意控制台使用的等宽字符的半角字符宽高比一般在1:2左右
	string charMap = " -=*n3W@"; //字符画所使用的字符，生成字符画时会根据单个像素的亮度来在这里寻找对应的字符

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); //获取句柄
	
	//隐藏光标
	CONSOLE_CURSOR_INFO cursorInfo = { 1, 0 };
	SetConsoleCursorInfo(hOut, &cursorInfo);
	
	//开启Windows控制台虚拟终端序列
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	
	string videoName;
	cout << "请输入视频路径：";
	getline(cin, videoName);
	VideoCapture video;
	video.open(videoName);
	
	//开几个数组用来存储视频信息和字符画
	int totalFrames = video.get(CAP_PROP_FRAME_COUNT);
	vector<vector<vector<Vec3b>>>color(totalFrames, vector<vector<Vec3b>>(outputSize.height, vector<Vec3b>(outputSize.width, { 0, 0, 0 })));
	vector<vector<vector<char>>>ch(totalFrames, vector<vector<char>>(outputSize.height, vector<char>(outputSize.width, 0)));
	
	//加载字符画
	cout << "Loading...";
	for (int n = 0; n < totalFrames; ++n) {
		Mat tmp0, tmp1;
		video.read(tmp0);
		resize(tmp0, tmp0, outputSize, 0, 0); //将每一帧原图的大小等比缩小到输出大小
		cvtColor(tmp0, tmp1, COLOR_BGR2GRAY);

		for (int i = 0; i < tmp0.rows; ++i)
			for (int j = 0; j < tmp0.cols; ++j) {
				color[n][i][j] = tmp0.at<Vec3b>(i, j); //获取每一帧的每个像素点的颜色
				ch[n][i][j] = charMap[tmp1.at<uchar>(i, j) * charMap.size() / 256]; //获取每一帧的每个像素点转换成字符
			}
	}

	system("cls");
	system(videoName.c_str()); //用默认播放器打开视频
	Sleep(200); //我使用的播放器打开视频时大概有200ms的延迟，所以我在这里Sleep了200ms
	
	Vec3b last = { 0, 0, 0 };
	clock_t currentTime = 0;
	clock_t totalTime = totalFrames / video.get(CAP_PROP_FPS) * CLOCKS_PER_SEC;
	clock_t startTime = clock();

	//播放字符动画
	while (1) {
		//获取当前帧
		currentTime = clock() - startTime;
		int current = 1.0 * currentTime / totalTime * totalFrames;
		
		if (current >= totalFrames) break;

		string output; //这里将输出的内容先存入字符串里，减少输出次数来提高输出速度
		output.reserve(outputSize.height * (outputSize.width + 1) * 12);
		
		SetConsoleCursorPosition(hOut, { 0, 0 }); //将光标移动至控制台的左上角
		
		for (int i = 0; i < outputSize.height; ++i) {
			for (int j = 0; j < outputSize.width; ++j) {
				Vec3b currentColor = color[current][i][j];
				if (currentColor != last) { //当前颜色和上一像素颜色不同时重新设置文本颜色，这里使用了控制台虚拟终端序列来设置终端颜色
					output += "\x1b[38;2;" + to_string(currentColor[2]) + ';' + to_string(currentColor[1]) + ';' + to_string(currentColor[0]) + 'm';
					last = currentColor;
				}
				output += ch[current][i][j];
			}
			output += '\n';
		}

		puts(output.c_str()); //打印字符画
	}

	puts("\x1b[0m"); //还原终端颜色
	return 0;
}