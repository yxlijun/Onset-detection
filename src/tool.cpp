#include "tool.h"
using namespace std;


void GetAllFileFromPath(std::string folderPath, std::vector<std::string> & onsetfiles, std::vector<std::string> &audiofiles, std::vector<std::string> &curpaths){
	_finddata_t FileInfo;
	string strfind = folderPath + "/*";
	intptr_t  Handle = _findfirst(strfind.c_str(), &FileInfo);
	if (Handle == -1L){
		_findclose(Handle);
		return;
	}
	do{
		if (FileInfo.attrib & _A_SUBDIR){
			if ((strcmp(FileInfo.name, ".") != 0) && (strcmp(FileInfo.name, "..") != 0)){
				string newPath = folderPath + "/" + FileInfo.name;
				GetAllFileFromPath(newPath, onsetfiles, audiofiles, curpaths);
			}
		}
		else{
			string newPath = folderPath + "/" + FileInfo.name;
			string path = FileInfo.name;
			int index = path.find(".mp3", 0);
			int index1 = path.find(".wav", 0);
			int index2 = path.find("mid", 0);
			int index3 = path.find("txt", 0);
			int index4 = path.find("frame",0);
			if (index != -1|| index1!=-1){
				audiofiles.push_back(newPath);
				curpaths.push_back(folderPath);
			}
			else if (index2 != -1 || (index3!=-1 && index4==-1)){
				onsetfiles.push_back(newPath);
			}
		}
	} while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);
}

bool findMidInStr(string str){
	string::size_type idx = str.find("mid");
	if (idx != string::npos)
		return true;
	else
		return false;
}

void initFrame(string audiopath){
	Py_Initialize();	//ʹ��python֮ǰ��Ҫ����Py_Initialize();����������г�ʼ��
	PyObject * pFunc = NULL;	// ��������
	PyObject * pModule = NULL;
	pModule = PyImport_ImportModule("SplitMp3");	//������Ҫ���õ��ļ���
	pFunc = PyObject_GetAttrString(pModule, "frame");	//������Ҫ���õĺ�����
	PyObject* args = PyTuple_New(1);
	PyObject* arg1 = Py_BuildValue("s", audiopath.c_str()); // �ַ�������
	PyTuple_SetItem(args, 0, arg1);
	PyEval_CallObject(pFunc, args);//���ú���
	Py_Finalize();//����Py_Finalize�������Py_Initialize���Ӧ��
}

std::vector<std::string> split(std::string str, std::string pattern){
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//��չ�ַ����Է������
	int size = str.size();
	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}