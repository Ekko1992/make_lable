#pragma once
#include "tinyxml.h"
#include <list>
#include <map>
#include "MyBox.h"

using namespace std;

typedef struct
{
	MyBox box;
	string name;
	map<string,string> attrs;
}Gt;

class MyGtXml
{
public:
	list<Gt> m_gtList;

public:
	MyGtXml(void);
	virtual ~MyGtXml(void);

	void Load(const string &xmlFile);
	void Save(const string &xmlFile);
	void PushBack(const MyBox &box, const string &name);
	void Remove(const Point &pt);
	bool Select(const Point &pt, list<Gt>::iterator &it);

	void ReadObject(TiXmlNode *node, Gt &gt);
	void ReadBox(TiXmlNode *node, MyBox &box);
	TiXmlElement* GetNodeObject(Gt &gt);
	TiXmlElement* GetNodeBox(MyBox &box);
};

