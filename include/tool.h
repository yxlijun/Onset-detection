#pragma once

#include <io.h>
#include <string>
#include <vector>
#include <Python.h>

void GetAllFileFromPath(std::string folderPath, std::vector<std::string> &onsetfiles, std::vector<std::string> &audiofiles, std::vector<std::string> &curpaths);


bool findMidInStr(std::string str);

void initFrame(std::string audiopath);

std::vector<std::string> split(std::string str, std::string pattern);