#include "Onsets.h"
#include <iostream>
#include "Evaluate.h"
#include "tool.h"

int main(){
	string diroot = "../data/MAPS/1-5";
	vector<string> onsetfiles;
	vector<string> audiofiles;
	vector<string> curpaths;
	GetAllFileFromPath(diroot, onsetfiles, audiofiles, curpaths);
	initFrame(diroot);
	for (int i = 0; i < audiofiles.size(); i++){
		Onset onsetdet(curpaths[i]);
		onsetdet.Init(true);
		onsetdet.detect();
		string onsetpath = curpaths[i] + "/onset.dat";
		onsetdet.saveResult(onsetpath);
		EvaluateAcc evaluate(onsetdet);
		evaluate.setMidi(findMidInStr(onsetfiles[i]));
		evaluate.EvaluateMark(onsetfiles[i]);
		vector<double> evaluateResult = evaluate.getEvaluateResult();
		cout << "precise:" << evaluateResult[0] << "\t recall:" << evaluateResult[1] << "\t F-measure:" << evaluateResult[2] << endl;
	}
	system("pause");
	return 0;
}


