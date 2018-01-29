#ifndef SPECTRALODF_H
#define SPECTRALODF_H

#include <string>
#include "kiss_fft.h"
#include <vector>
#include <resample.h>
#include <algorithm>
#include <cmath>
#include <numeric>

using namespace std;
class SpectralODF
{
public:
	SpectralODF();
	~SpectralODF();

	void ReadFrameFile(const string &framePath);   //读取MP3分帧后的数据

	void spectrumOfFrame(double* xFrame, double* spectrum);          // 计算频谱

	void hammingWindow(double *w, int windowSize);         // 计算汉明窗

	void CalSpectrum();


	void SpectralFlux();

	vector<double> getSFResult();

	vector<vector<double>> getSpectrum();

	void AdaptiveWhiten();

	void spectrumLog();

	double getfps();

	vector<double> getSFresultOnline();

	double CalSpectrumOnLine(int frameindex);

	int getframeSum();

	void setOnLine(bool online);

	void frequencies();

	vector<double> triang(int start, int mid, int stop);

	void Filter(double fs = 44100,double fminx = 27.5,double fmaxx=16000);
private:
	int FFTSIZE;             //fft长度
	int SPECTRUMLENGTH;              // 谱长度（fftSize/2+1）
	int HOPSIZE;                    // 帧移
	int FRAMESIZE;                  // 帧长（samples）
	int FrameSum;                //MP3一共的帧数

	int floors;
	int relaxation;

	double fps;
	int mul;

	bool onLine;
	const int FrameLength = 4096;             //每一帧的长度
	vector<vector<double>> xFrame;                  //原始音频数据
	vector<vector<double>> spectrum;               //整首MP3的频谱
	const string windowpath = "../config/window4096_1024.txt";
	vector<double> SFResult;
	vector<double> SFResultOnline;
	vector<vector<double>> spectrumOnline;
	vector<double> curspectrum;
	vector<double> frequency;

	vector<vector<double>> filterbank;
};


#endif