#include "SpectralODF.h"
#include <iostream>
#include <fstream>
using namespace std;

SpectralODF::SpectralODF(){
	FFTSIZE = 1024;
	SPECTRUMLENGTH = 513;
	HOPSIZE = 110;
	FRAMESIZE = 1024;
	floors = 5;
	relaxation = 10;
	fps = static_cast<double>(44100) / 512;
	mul = 10;
	Filter();
}

SpectralODF::~SpectralODF(){

}

double SpectralODF::getfps(){
	return fps;
}


void SpectralODF::ReadFrameFile(const string &framePath){
	ifstream FrameStream(framePath);
	FrameSum = 0;
	string tmp;
	while (getline(FrameStream, tmp, '\n')){
		FrameSum++;
	}
	FrameStream.close();
	xFrame.resize(FrameSum);
	for (int i = 0; i < FrameSum; i++){
		xFrame[i].resize(FrameLength);
	}
	FILE *xFrameFile;
	if ((xFrameFile = fopen(framePath.c_str(), "rb")) == NULL){
		printf("cannot open!\n");
		exit(1);
	}
	for (int i = 0; i < FrameSum; ++i){
		for (int j = 0; j < FrameLength; ++j){
			fscanf(xFrameFile, "%lf", &xFrame[i][j]);
		}
	}
	fclose(xFrameFile);
	remove(framePath.c_str());
}
void SpectralODF::CalSpectrum(){
	vector<vector<double>> outframe;
	outframe.resize(FrameSum);
	for (int i = 0; i < FrameSum; i++){
		outframe[i] = resample(xFrame[i], 1024, 4096, windowpath);
	}
	spectrum.resize(FrameSum);
	for (int i = 0; i < FrameSum; i++){
		spectrum[i].resize(SPECTRUMLENGTH);
	}
	for (int i = 0; i < FrameSum; i++){
		spectrumOfFrame(&outframe[i][0], &spectrum[i][0]);
	}
	vector<vector<double>> temp(FrameSum);
	for (int i = 0; i < FrameSum; i++){
		temp[i].resize(filterbank[0].size());
	}
	for (int i = 0; i < FrameSum; i++){
		for (int j = 0; j < filterbank[j].size(); j++){
			for (int k = 0; k <SPECTRUMLENGTH; k++){
				temp[i][j] += spectrum[i][k] * filterbank[k][j];
			}
		}
	}
	spectrum = temp;
	spectrumLog();
	SpectralFlux();
}

void SpectralODF::spectrumOfFrame(double* xFrame, double* spectrum){
	int i;
	double* win = (double*)calloc(FRAMESIZE, sizeof(double));
	hammingWindow(win, FRAMESIZE);
	//加窗
	double* xFft = (double*)calloc(FFTSIZE, sizeof(double));
	for (i = 0; i < FRAMESIZE; ++i)
	{
		xFft[i] = xFrame[i] * win[i];
	}
	if (FFTSIZE > FRAMESIZE) //超过的部分补0
	{
		for (i = FRAMESIZE; i < FFTSIZE; ++i)
		{
			xFft[i] = 0.0;
		}
	}
	free(win); win = NULL;

	//kiss-fft
	//input
	kiss_fft_cpx* kiss_xFft = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx)*FFTSIZE);
	for (i = 0; i < FFTSIZE; ++i)
	{
		kiss_xFft[i].r = xFft[i];
		kiss_xFft[i].i = 0.0;
	}
	free(xFft); xFft = NULL;

	//output
	kiss_fft_cpx* fftFrame = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx)*FFTSIZE);
	kiss_fft_cfg cfg = kiss_fft_alloc(FFTSIZE, 0, 0, 0);

	kiss_fft(cfg, kiss_xFft, fftFrame);

	//计算频谱振幅
	for (i = 0; i < SPECTRUMLENGTH; ++i)
	{
		spectrum[i] = sqrt(fftFrame[i].r * fftFrame[i].r + fftFrame[i].i * fftFrame[i].i); // 计算幅值
	}

	free(kiss_xFft); free(cfg);
	free(fftFrame); fftFrame = NULL;
}

void SpectralODF::hammingWindow(double *w, int windowSize){
	int i;
	double pi = 3.14159265358979323846;
	for (i = 0; i < windowSize; ++i) {
		w[i] = 0.54 - 0.46 * cos(2 * pi * i / (windowSize - 1));
	}
}

void SpectralODF::SpectralFlux(){
	if (!onLine){
		SFResult.push_back(0.0);
		for (int i = 1; i < spectrum.size(); i++){
			vector<double> diff;
			for (int j = 0; j < spectrum[i].size(); j++){
				diff.push_back(spectrum[i][j] - spectrum[i - 1][j]);
			}
			for (int j = 0; j < spectrum[i].size(); j++){
				if (diff[j] < 0)
					diff[j] = 0;
			}
			double sum = accumulate(diff.begin(), diff.end(), 0.0);
			SFResult.push_back(sum);
		}
	}
	else{
		if (SFResultOnline.empty()){
			SFResultOnline.push_back(0.0);
			return;
		}
		else{
			int SpecOnLineSize = spectrumOnline.size() - 1;
			vector<double> diff;
			for (int i = 0; i < spectrumOnline[SpecOnLineSize].size(); i++){
				diff.push_back(spectrumOnline[SpecOnLineSize][i] - spectrumOnline[SpecOnLineSize - 1][i]);
			}
			for (int j = 0; j < spectrumOnline[SpecOnLineSize].size(); j++){
				if (diff[j] < 0)
					diff[j] = 0;
			}
			double sum = accumulate(diff.begin(), diff.end(), 0.0);
			SFResultOnline.push_back(sum);
		}
	}
	
}

vector<double> SpectralODF::getSFResult(){
	return SFResult;
}

vector<vector<double>> SpectralODF::getSpectrum(){
	return spectrum;
}

void SpectralODF::AdaptiveWhiten(){
	double x = -6 * relaxation / static_cast<double>(fps);
	double mem_coeff = pow(10,x);
	vector<vector<double>> specModel;
	specModel.resize(FrameSum);
	for (int i = 0; i < FrameSum; i++){
		specModel.resize(SPECTRUMLENGTH);
	}
	for (int i = 0; i < FrameSum; i++){
		vector<double> spec_floor(SPECTRUMLENGTH);
 		for (int j = 0; j < SPECTRUMLENGTH; j++){
			if (spectrum[i][j]>floors)
				spec_floor[j] = spectrum[i][j];
			else
				spec_floor[j] = floors;
			if (i > 0){
				if (spec_floor[j] > mem_coeff*specModel[i - 1][j])
					specModel[i][j] = spec_floor[j];
				else
					specModel[i][j] = mem_coeff*specModel[i - 1][j];
			}
			else specModel[i] = spec_floor;
		}
	}
	for (int i = 0; i < FrameSum; i++){
		for (int j = 0; j < SPECTRUMLENGTH; j++)
			spectrum[i][j] /= (static_cast<double>(specModel[i][j]));
	}
}

void SpectralODF::spectrumLog(){
	if (!onLine){
		for (int i = 0; i < spectrum.size(); i++){
			for (int j = 0; j < spectrum[i].size(); j++){
				spectrum[i][j] = log10(mul*spectrum[i][j] + 1);
			}
		}
	}
	else{
		for (int i = 0; i < curspectrum.size(); i++){
			curspectrum[i] = log10(mul*curspectrum[i] + 1);
		}
	}
}

vector<double> SpectralODF::getSFresultOnline(){
	return SFResultOnline;
}

double SpectralODF::CalSpectrumOnLine(int frameindex){
	vector<double> outframe = resample(xFrame[frameindex], 1024, 4096, windowpath);
	curspectrum.resize(SPECTRUMLENGTH);
	spectrumOfFrame(&outframe[0], &curspectrum[0]);
	vector<double> spectemp;
	spectemp.resize(filterbank[0].size());
	for (int j = 0; j < filterbank[j].size(); j++){
		for (int k = 0; k <SPECTRUMLENGTH; k++){
			spectemp[j] += curspectrum[k] * filterbank[k][j];
		}
	}
	curspectrum = spectemp;
	spectrumLog();
	spectrumOnline.push_back(curspectrum);
	SpectralFlux();
	return SFResultOnline.back();
}

int SpectralODF::getframeSum(){
	return FrameSum;
}

void SpectralODF::setOnLine(bool online){
	onLine = online;
}


void SpectralODF::frequencies(){
	double fminx = 27.5;
	double fmaxx = 16000;
	int bands = 12;
	double a = 440;
	double factor = pow(2,1.0/bands);
	double freq = a;
	frequency.push_back(freq);
	while (freq<=fmaxx){
		freq *= factor;
		frequency.push_back(freq);
	}
	freq = a;
	while (freq>=fminx){
		freq /= factor;
		frequency.push_back(freq);
	}
	sort(frequency.begin(), frequency.end());
}

vector<double> SpectralODF::triang(int start, int mid, int stop){
	double height = 1.0;
	vector<double> triang_filter(stop - start);
	for (int i = 0; i < mid-start; i++){
		triang_filter[i] = i * (height / (mid - start));
	}
	double ave = height / (stop-mid);
	for (int i = 0; i < stop-mid; i++){
		triang_filter[i+mid-start] = height - i*ave;
	}
	return triang_filter;
}

void SpectralODF::Filter(double fs, double fminx, double fmaxx){
	if (fmaxx > fs / 2)
		fmaxx = fs / 2;
	frequencies();
	double factor = (fs/2.0) / SPECTRUMLENGTH;
	for (int i = 0; i < frequency.size(); i++){
		frequency[i] = round(frequency[i] / factor);
	}
	vector<double>::iterator pos = unique(frequency.begin(), frequency.end());
	frequency.erase(pos, frequency.end());
	vector<int> frequencyTemp;
	for (int i = 0; i < frequency.size(); i++){
		if (frequency[i] < SPECTRUMLENGTH)
			frequencyTemp.push_back(frequency[i]);
	}
	int bands = frequencyTemp.size() - 2;
	filterbank.resize(SPECTRUMLENGTH);
	for (int i = 0; i < SPECTRUMLENGTH; i++){
		filterbank[i].resize(bands);
	}
	for (int i = 0; i < bands; i++){
		int start = frequencyTemp[i];
		int mid = frequencyTemp[i + 1];
		int stop = frequencyTemp[i + 2];
		vector<double> triang_filter = triang(start, mid, stop);
		for (int j = start; j <stop; j++){
			filterbank[j][i] = triang_filter[j - start];
		}
	}
}