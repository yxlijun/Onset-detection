#include "Evaluate.h"



EvaluateAcc::EvaluateAcc(Onset onset1) :onsetdet(onset1){

}
EvaluateAcc::~EvaluateAcc(){

}
void EvaluateAcc::readMidi(string midfile){
	Options options;
	MidiFile midifile;
	midifile.read(midfile);
	for (int event = 0; event < midifile[1].size(); event++) {
		int startmark = midifile[1][event][0];
		if (startmark != 128 && startmark != 144)
			continue;
		if (startmark == 144){
			double onset = midifile.getTimeInSeconds(midifile[1][event].tick);
			stdMidiOnset.push_back(onset);
		}
	}
	sort(stdMidiOnset.begin(), stdMidiOnset.end());
	vector<double>::iterator pos = unique(stdMidiOnset.begin(), stdMidiOnset.end());
	stdMidiOnset.erase(pos, stdMidiOnset.end());
}

void EvaluateAcc::EvaluateMark(string file){
	vector<double> detResult = onsetdet.getdecectionResult();
	vector<double> stdOnset;
	if (ismidi){
		readMidi(file);
		stdOnset = stdMidiOnset;
	}
	else {
		readStdOnsetfile(file);
		stdOnset = stdTxtOnset;
	}
	int count = 0;
	for (int i = 0; i < detResult.size(); i++){
		for (int j = 0; j < stdOnset.size(); j++){
			if (abs(detResult[i] - stdOnset[j]) <= 0.05){
				count++;
				break;
			}
		}
	}
	double precissRate = static_cast<double>(count) / (static_cast<double>(detResult.size()));
	double recall = static_cast<double>(count) / (static_cast<double>(stdOnset.size()));
	double Frate = 2 * precissRate*recall / (precissRate + recall);
	evaluateResult.push_back(precissRate);
	evaluateResult.push_back(recall);
	evaluateResult.push_back(Frate);
}


vector<double> EvaluateAcc::getEvaluateResult(){
	return evaluateResult;
}

void EvaluateAcc::readStdOnsetfile(string onsetfile){
	ifstream fin(onsetfile);
	string line;
	while (getline(fin, line)){
		vector<string> result = split(line, "\t");
		if (result.size() > 1){
			if (result[0] != "OnsetTime"){
				stdTxtOnset.push_back(stod(result[0]));
			}
		}
		else{
			stdTxtOnset.push_back(stod(result[0]));
		}
	}
	vector<double>::iterator pos = unique(stdTxtOnset.begin(), stdTxtOnset.end());
	stdTxtOnset.erase(pos, stdTxtOnset.end());
	vector<vector<double>> temp1;
	for (int i = 0; i < stdTxtOnset.size(); i++){
		int j;
		vector<double> tmp;
		for (j = i; j < stdTxtOnset.size(); j++){
			if (abs(stdTxtOnset[i] - stdTxtOnset[j]) <= 0.020){
				tmp.push_back(stdTxtOnset[j]);
			}
			else break;
		}
		temp1.push_back(tmp);
		i = j;
		i--;
	}
	vector<double>().swap(stdTxtOnset);
	for (int i = 0; i < temp1.size(); i++){
		double sum = accumulate(temp1[i].begin(), temp1[i].end(), 0.0);
		double avg = sum / temp1[i].size();
		stdTxtOnset.push_back(avg);
	}
	fin.close();
}

void EvaluateAcc::setMidi(bool midi){
	ismidi = midi;
}