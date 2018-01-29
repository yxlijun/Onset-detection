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

	void ReadFrameFile(const string &framePath);   //��ȡMP3��֡�������

	void spectrumOfFrame(double* xFrame, double* spectrum);          // ����Ƶ��

	void hammingWindow(double *w, int windowSize);         // ���㺺����

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
	int FFTSIZE;             //fft����
	int SPECTRUMLENGTH;              // �׳��ȣ�fftSize/2+1��
	int HOPSIZE;                    // ֡��
	int FRAMESIZE;                  // ֡����samples��
	int FrameSum;                //MP3һ����֡��

	int floors;
	int relaxation;

	double fps;
	int mul;

	bool onLine;
	const int FrameLength = 4096;             //ÿһ֡�ĳ���
	vector<vector<double>> xFrame;                  //ԭʼ��Ƶ����
	vector<vector<double>> spectrum;               //����MP3��Ƶ��
	const string windowpath = "../config/window4096_1024.txt";
	vector<double> SFResult;
	vector<double> SFResultOnline;
	vector<vector<double>> spectrumOnline;
	vector<double> curspectrum;
	vector<double> frequency;

	vector<vector<double>> filterbank;
};


#endif