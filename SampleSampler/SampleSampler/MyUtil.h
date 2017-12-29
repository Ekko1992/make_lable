#ifndef __MY_UTIL_H__
#define __MY_UTIL_H__

#include <string>
#include <vector>
#include <direct.h> //_mkdir������ͷ�ļ�
#include <io.h>     //_access������ͷ�ļ�
#include <Windows.h>

using namespace std;

void CreateDir(const char *dir);
void GetAllFileNames(const string &dir, vector<string> &exts, vector<string> &fileNames);
FILE* GetPrmsPtr(FILE *fpTop, const string &item);
string int2string(int a);
void StringSplit(const string &str, vector<string> &ret);

#endif

