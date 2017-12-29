#include "MyGtXml.h"
#include "MyUtil.h"

MyGtXml::MyGtXml(void)
{
}

MyGtXml::~MyGtXml(void)
{
}

void MyGtXml::Load(const string &xmlFile)
{
	m_gtList.clear();//必须先清零

	TiXmlDocument xml;
	if( !xml.LoadFile( xmlFile ) )
		return;
	
	Gt gt = {MyBox(0,0,0,0), "NULL"};
	TiXmlNode *node;
	TiXmlElement *root = xml.RootElement();//annotation

	if( NULL != root )
		node = root->FirstChild();
	
	for( ; NULL != node; node = node->NextSibling() )
	{
		if( 0 == strcmp(node->Value(), "object") )
		{
			Gt obj;
			ReadObject( node->FirstChild(), obj );	
			m_gtList.push_back( obj );
		}
	}
}

void MyGtXml::Save(const string &xmlFile)
{
	TiXmlDocument xml;
	TiXmlElement *node1;

	TiXmlElement *root = new TiXmlElement( "annotion" );
	xml.LinkEndChild( root );

	list<Gt>::iterator it;
	for( it = m_gtList.begin(); it != m_gtList.end(); it++ )
	{
		node1 = GetNodeObject( *it );
		root->LinkEndChild( node1 );
	}
	xml.SaveFile( xmlFile );
}

void MyGtXml::PushBack(const MyBox &box, const string &name)
{
	Gt gt = {box, name};
	m_gtList.push_back( gt );
}

void MyGtXml::Remove(const Point &pt)
{
	list<Gt>::iterator it;
	for( it = m_gtList.begin(); it != m_gtList.end(); )
	{		
		if( (*it).box.Enclose(pt) )
		{
			it = m_gtList.erase( it );//从列表中删除选中的框
		}
		else
		{
			it++;
		}
	}
}

bool MyGtXml::Select(const Point &pt, list<Gt>::iterator &it)
{
	list<Gt>::iterator it0;
	for( it0 = m_gtList.begin(); it0 != m_gtList.end(); it0++ )
	{		
		if( it0->box.Enclose(pt) )
		{
			it = it0;
			return true;
		}
	}
	return false;
}

void MyGtXml::ReadObject(TiXmlNode *node, Gt &gt)
{
	for( ; NULL != node; node = node->NextSibling() )
	{
		if( 0 == strcmp(node->Value(), "bndbox") )
		{
			ReadBox( node->FirstChild(), gt.box );
		}
		else if( 0 == strcmp(node->Value(), "name") )
		{
			gt.name = node->FirstChild()->Value();
		}
		else
		{
			string attr = node->Value();
			string val = node->FirstChild()->Value();
			gt.attrs.insert(pair<string,string>(attr,val));
		}
	}
}

void MyGtXml::ReadBox(TiXmlNode *node, MyBox &box)
{
	for( ; NULL != node; node = node->NextSibling() )
	{
		if( 0 == strcmp(node->Value(), "xmin") )
		{
			sscanf_s( node->FirstChild()->Value(), "%d", &box.m_xmin );
		}
		else if( 0 == strcmp(node->Value(), "ymin") )
		{
			sscanf_s( node->FirstChild()->Value(), "%d", &box.m_ymin );
		}
		else if( 0 == strcmp(node->Value(), "xmax") )
		{
			sscanf_s( node->FirstChild()->Value(), "%d", &box.m_xmax );
		}
		else if( 0 == strcmp(node->Value(), "ymax") )
		{
			sscanf_s( node->FirstChild()->Value(), "%d", &box.m_ymax );
		}
	}
}

TiXmlElement* MyGtXml::GetNodeObject(Gt &gt)
{
	TiXmlElement *node1, *node2;
	TiXmlText *text;

	node1 = new TiXmlElement( "object" );

	node2 = new TiXmlElement( "name" );
	node1->LinkEndChild( node2 );
	text = new TiXmlText( gt.name );
	node2->LinkEndChild( text );
	node2 = GetNodeBox( gt.box );
	node1->LinkEndChild( node2 );

	map<string,string>::iterator it;
	map<string,string> &attrs = gt.attrs;
	for(it = attrs.begin(); it != attrs.end(); it++)
	{
		node2 = new TiXmlElement(it->first);
		node1->LinkEndChild(node2);
		text = new TiXmlText(it->second);
		node2->LinkEndChild(text);
	}

	return node1;
}

TiXmlElement* MyGtXml::GetNodeBox( MyBox &box )
{
	TiXmlElement *node1, *node2;
	TiXmlText *text;

	node1 = new TiXmlElement( "bndbox" );

	node2 = new TiXmlElement( "xmin" );
	node1->LinkEndChild( node2 );
	text = new TiXmlText( int2string(box.m_xmin) );
	node2->LinkEndChild( text );

	node2 = new TiXmlElement( "ymin" );
	node1->LinkEndChild( node2 );
	text = new TiXmlText( int2string(box.m_ymin) );
	node2->LinkEndChild( text );

	node2 = new TiXmlElement( "xmax" );
	node1->LinkEndChild( node2 );
	text = new TiXmlText( int2string(box.m_xmax) );
	node2->LinkEndChild( text );

	node2 = new TiXmlElement( "ymax" );
	node1->LinkEndChild( node2 );
	text = new TiXmlText( int2string(box.m_ymax) );
	node2->LinkEndChild( text );

	return node1;
}


