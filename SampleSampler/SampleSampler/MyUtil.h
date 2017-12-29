#ifndef __MY_UTIL_H__
#define __MY_UTIL_H__

#include <string>
#include <vector>
#include <direct.h> //_mkdir函数的头文件
#include <io.h>     //_access函数的头文件
#include <Windows.h>

using namespace std;

void CreateDir(const char *dir);
void GetAllFileNames(const string &dir, vector<string> &exts, vector<string> &fileNames);
FILE* GetPrmsPtr(FILE *fpTop, const string &item);
string int2string(int a);
void StringSplit(const string &str, vector<string> &ret);

#endif

