#pragma once

#include "MidiFile.h"
#include "Options.h"
#include <string>
#include <algorithm>
#include <Onsets.h>
#include "tool.h"


class EvaluateAcc
{
public:
	EvaluateAcc(Onset onset1);
	~EvaluateAcc();
	void readMidi(string midfile);

	void EvaluateMark(string file);

	vector<double> getEvaluateResult();

	void readStdOnsetfile(string onsetfile);

	void setMidi(bool midi);

private:
	vector<double> stdMidiOnset;
	Onset onsetdet;
	vector<double> evaluateResult;

	vector<double> stdTxtOnset;

	bool ismidi;
};

