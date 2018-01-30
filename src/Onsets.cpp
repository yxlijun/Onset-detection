#include "Onsets.h"
#include <iostream>
using namespace std;

Onset::Onset(string audiopath){
	this->audiopath = audiopath;
	pre_max = 6;
	pre_avg = 20;
	combine = 10;
	threshold = 2.5;
	last_onset = 0;
	LessCombine = true;
}
Onset::~Onset(){

}

void Onset::Init(bool online){
	fps = spectralOdf.getfps();
	string framepath = audiopath + "/frame.txt";
	spectralOdf.ReadFrameFile(framepath);
	spectralOdf.setOnLine(online);
	setOnLine(online);
	if (!online){
		spectralOdf.CalSpectrum();
		vector<vector<double>> spectrum = spectralOdf.getSpectrum();
		SFResult = spectralOdf.getSFResult();
	}
	else{
		int frameSum = spectralOdf.getframeSum();
		for (int i = 0; i < frameSum; i++){
			Timer timer;
			timer.Tic();
			cursfresult = spectralOdf.CalSpectrumOnLine(i);
			SFResult.push_back(cursfresult);
			detect();
			timer.Toc();
			//std::cout << "cost time:" << timer.Elasped() << "ms" << endl;
		}
		int x;
	}
	return;
}

void Onset::detect(){
	if (!online){
		for (int i = 0; i < SFResult.size(); i++){
			int start_max = (i - pre_max) >= 0 ? i - pre_max : 0;
			int start_avg = (i - pre_avg) >= 0 ? i - pre_avg : 0;
			vector<double> maxfilter;
			vector<double> avgfilter;
			for (int j = start_max; j <= i; j++){
				maxfilter.push_back(SFResult[j]);
			}
			for (int j = start_max; j <= i; j++){
				avgfilter.push_back(SFResult[j]);
			}
			double mov_max = *max_element(maxfilter.begin(), maxfilter.end());
			double mov_avg = accumulate(avgfilter.begin(), avgfilter.end(), 0.0) / static_cast<double>(avgfilter.size());
			bool curmax = SFResult[i] == mov_max;
			bool curavg = SFResult[i] >= (mov_avg + threshold);
			bool currange = i > (last_onset + combine);
			if (i < combine){
				if (curavg && curmax && LessCombine){
					double onset = static_cast<double>(i) / fps;
					dectectionResult.push_back(onset);
					LessCombine = false;
					last_onset = i;
				}
			}
			if (curmax && curavg && currange){
				double onset = static_cast<double>(i) / fps;
				dectectionResult.push_back(onset);
				last_onset = i;
			}
		}
	}
	else{
		int index = SFResult.size() - 1;
		int start_max = (index - pre_max) >= 0 ? index - pre_max : 0;
		int start_avg = (index - pre_avg) >= 0 ? index - pre_avg : 0;
		vector<double> maxfilter;
		vector<double> avgfilter;
		for (int j = start_max; j <= index; j++){
			maxfilter.push_back(SFResult[j]);
		}
		for (int j = start_max; j <= index; j++){
			avgfilter.push_back(SFResult[j]);
		}
		double mov_max = *max_element(maxfilter.begin(), maxfilter.end());
		double mov_avg = accumulate(avgfilter.begin(), avgfilter.end(), 0.0) / static_cast<double>(avgfilter.size());
		bool curmax = cursfresult == mov_max;
		bool curavg = cursfresult >= (mov_avg + threshold);
		bool currange = index > (last_onset + combine);
		if (index < combine){
			if (curavg && curmax && LessCombine){
				double onset = static_cast<double>(index) / fps;
				dectectionResult.push_back(onset);
				LessCombine = false;
				last_onset = index;
			}
		}
		if (curmax && curavg && currange){
			double onset = static_cast<double>(index) / fps;
			dectectionResult.push_back(onset);
			last_onset = index;
		}

	}
	
}

void Onset::saveResult(string path){
	ofstream fout;
	fout.open(path);
	for (int i = 0; i < dectectionResult.size(); i++){
		fout << dectectionResult[i] << endl;
	}
	fout.close();
}

vector<double> Onset::getdecectionResult(){
	return dectectionResult;
}

void Onset::setOnLine(bool online){
	this->online = online;
}
