#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>
#include "MyGtXml.h"

using namespace std;
using namespace cv;

class MySampler
{
public:
	Mat m_imgSrc;
	Mat m_imgDraw;	

	MyGtXml m_gtXml;
	MyBox m_boxDraw;
	Size m_gtSize;

	vector<string> m_names;
	map<string,Scalar> m_nameColors;	
	vector<string> m_attributes;
	map<string,Scalar> m_attrColors;
	int m_tipId;
	int m_defaultNameId;

	bool m_bSelected;
	list<Gt>::iterator m_selectedGtIt;

	list<Gt> m_gtListLastFrm;
	
public:
	MySampler(void);
	virtual ~MySampler(void);

	void GetConfig();
	void GetSamples();

	void DrawGt(Mat &img);
	void DrawGt(Mat &img, Gt &gt);

	void PrintTips(vector<string> &strs);
	void InsertName(Gt &gt);
	void InsertNameFromInput(Gt &gt);
	void InsertAttrs(Gt &gt);
};


