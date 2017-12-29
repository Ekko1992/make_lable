#include "MySampler.h"
#include "MyBox.h"
#include "MyUtil.h"

extern int width = 1920/2;
extern int height = 1080/2;

MySampler::MySampler(void)
{
	GetConfig();
	m_tipId = 0;
	m_defaultNameId = 0;
	m_bSelected = false;
}


MySampler::~MySampler(void)
{
}

void MySampler::GetConfig()
{
	FILE *fpTop, *fp;
	fopen_s(&fpTop, "Config.txt", "rt");
	if(NULL == fpTop)
	{
		printf("cannot open Config.txt\n");
		system("pause");
	}

	int b, g, r;
	char buffer[255];

	//sample size
	fp = GetPrmsPtr(fpTop, "SampleSize");
	if(NULL != fp)
	{
		fscanf_s(fp, "%d %d", &m_gtSize.width, &m_gtSize.height);
		fscanf_s(fp, "%*[^\n]");
	}

	//names
	fp = GetPrmsPtr(fpTop, "Names");
	if(NULL != fp)
	{
		do 
		{
			fscanf_s(fp, "%d\t%d\t%d\t%s", &b, &g, &r, buffer, 255);
			fscanf_s(fp, "%*[^\n]");
			m_names.push_back(buffer);
			m_nameColors.insert(pair<string, Scalar>(buffer,Scalar(b,g,r)));
		} 
		while(0 != strcmp(buffer, "NULL"));	
	}
	else
	{
		m_names.push_back("NULL");
		m_nameColors.insert(pair<string, Scalar>("NULL",Scalar(0,0,255)));
	}

	//attributes
	fp = GetPrmsPtr(fpTop, "Attributes");
	if(NULL != fp)
	{
		do 
		{
			fscanf_s(fp, "%d\t%d\t%d\t%s", &b, &g, &r, buffer, 255);
			fscanf_s(fp, "%*[^\n]");
			m_attributes.push_back(buffer);
			m_attrColors.insert(pair<string, Scalar>(buffer,Scalar(b,g,r)));
		} 
		while(0 != strcmp(buffer, "NULL"));	
	}
	else
	{
		m_attributes.push_back("NULL");
		m_attrColors.insert(pair<string, Scalar>("NULL",Scalar(0,0,255)));
	}

	fclose(fpTop);
}

void OnMouse( int event, int x, int y, int flag, void *param )
{
	static int click = 0;
	static Point pt;

	MySampler *sampler = (MySampler *)param;
	Mat &src = sampler->m_imgSrc;
	Mat &draw = sampler->m_imgDraw;

	MyGtXml &gtXml = sampler->m_gtXml;
	Size &gtSize = sampler->m_gtSize;
	MyBox &boxDraw = sampler->m_boxDraw;

	bool &bSelected = sampler->m_bSelected;
	list<Gt>::iterator &selectedGtIt = sampler->m_selectedGtIt;

	bool bFixedSize = (gtSize.width>0) && (gtSize.height>0);
	float ratioWpH = bFixedSize ? (float)gtSize.width/(float)gtSize.height : 0.0f;

	int height = src.rows;
	int width = src.cols;
	Scalar &color = sampler->m_nameColors.find("NULL")->second;
	string &name = sampler->m_names[sampler->m_defaultNameId];
	
	if( CV_EVENT_MOUSEMOVE == event )
	{
		src.copyTo( draw );
		sampler->DrawGt(draw);
		if( click < 2 )
		{
			line( draw, Point(x,0), Point(x,height-1), color );
			line( draw, Point(0,y), Point(width-1, y), color );
			if( 1 == click )
			{		
				rectangle( draw, pt, Point(x,y), color );
			}	
		}
		else
		{
			MyBox box( Point(x,y), Size(boxDraw.Width(),boxDraw.Height()) );
			box.MoveIntoSight( MyBox(0,0,width-1,height-1) );
			rectangle( draw, Point(box.m_xmin,box.m_ymin), Point(box.m_xmax,box.m_ymax), color );
			boxDraw = box;
		}

		if(bSelected)
		{
			sampler->DrawGt(draw, *selectedGtIt);
		}
		imshow( "image", draw );
	}
	else if( CV_EVENT_LBUTTONDOWN == event )
	{
		//printf( "leftButtonDown: x = %d, y = %d, click = %d\n", x, y, click );
		bSelected = false;
		if( 0 == click )
		{
			click = 1;
			pt.x = x;
			pt.y = y;
		}
	}
	else if( (CV_EVENT_LBUTTONUP == event) )
	{
		//printf( "leftButtonUp: x = %d, y = %d, click = %d\n", x, y, click );
		if( 1 == click )
		{
			click = 0;
			boxDraw = MyBox( min(pt.x,x), min(pt.y,y), max(pt.x,x), max(pt.y,y) );
			if( (boxDraw.Height()>=8) && (boxDraw.Width()>=8) )//目标框必须大于8x8
			{
				if(!bFixedSize)
				{
					gtXml.PushBack(boxDraw, name);
				}
				else
				{
					click = 2;
					boxDraw.Shrink(ratioWpH);
				}
			}
		}
		else if( 2 == click )
		{
			click = 0;
			gtXml.PushBack(boxDraw, name);
		}

		src.copyTo( draw );
		sampler->DrawGt(draw);
		imshow( "image", draw );
	}
	else if( CV_EVENT_LBUTTONDBLCLK == event )
	{
		//printf( "leftButtonDoubleClick: x = %d, y = %d, click = %d\n", x, y, click );
		click = 0;
		gtXml.Remove( Point(x,y) );//删除选中的框
	}
	else if( CV_EVENT_RBUTTONDOWN == event )
	{
		bSelected = gtXml.Select(Point(x,y), selectedGtIt);
		if( bSelected )
		{
			sampler->DrawGt(draw, *selectedGtIt);
			waitKey(10);
		}
	}
}

void MySampler::GetSamples()
{
	ifstream in("imgfiles.txt");
	if(!in.is_open())
	{
		cout<<"cannot open imgfiles.txt to read files."<<endl;
		system("pause");
		return;
	}

	vector<string> imgfiles;
	string temp;
	cout<<"read image file: ";
	while(!in.eof())
	{
		in>>temp;
		imgfiles.push_back(temp);
	}
	in.close();
	cout<<imgfiles.size()<<" done"<<endl;

	int imgidx;
	Scalar &color = m_nameColors.find("NULL")->second;
	for(imgidx = 0; imgidx < (int)imgfiles.size(); imgidx++)
	{
		string &imgFile = imgfiles[imgidx];
		cout<<imgFile<<endl;

		int m0 = imgFile.rfind('\\');
		int m1 = imgFile.rfind('.');
		string gtDir = imgFile.substr(0, m0) + "\\gtXml";
		string gtFile = gtDir + "\\" + imgFile.substr(m0+1, m1-m0-1) + ".xml";
		CreateDir(gtDir.c_str());
		
		m_imgSrc = imread( imgFile );

		resize(m_imgSrc,m_imgSrc,Size(width,height));//select the size to show
		m_gtXml.Load( gtFile );
		//clear selected information or there will be a crash(list iterator will lost its object)
		m_bSelected = false;
		m_selectedGtIt = m_gtXml.m_gtList.begin();

		m_imgSrc.copyTo( m_imgDraw );
		DrawGt(m_imgDraw);
		imshow( "image", m_imgDraw );

		setMouseCallback( "image", OnMouse, (void *)this );

		while( 1 )
		{
			int key = waitKey( 1000 );
			if( key < 0 )
				continue;

			if( 'f' == key )
			{
				m_gtXml.Save( gtFile );
				m_gtListLastFrm = m_gtXml.m_gtList;
				break;
			}
			else if( 'e' == key )
			{
				if(m_bSelected)
				{
					MyBox box = m_selectedGtIt->box;
					box.Resize( 1.0f-1.0f/32.0f );
					if( (box.Height()>=8) && (box.Width()>=8) )
					{
						m_selectedGtIt->box = box;
						m_imgSrc.copyTo( m_imgDraw );
						DrawGt(m_imgDraw);
						DrawGt(m_imgDraw, *m_selectedGtIt);
						imshow( "image", m_imgDraw );
						waitKey( 10 );
					}
				}
				else
				{
					MyBox box = m_boxDraw;
					box.Resize( 1.0f-1.0f/32.0f );
					if( (box.Height()>=8) && (box.Width()>=8) )
					{
						m_boxDraw = box;
						m_imgSrc.copyTo( m_imgDraw );
						DrawGt(m_imgDraw);
						rectangle(m_imgDraw, Point(box.m_xmin, box.m_ymin), Point(box.m_xmax, box.m_ymax), color);
						imshow( "image", m_imgDraw );
						waitKey( 10 );
					}
				}
			}
			else if( 'r' == key )
			{
				if(m_bSelected)
				{
					MyBox box = m_selectedGtIt->box;
					box.Resize( 1.0f + 1.0f/MIN(32,MIN(box.Height()-1,box.Width()-1)) );
					if( box.MoveIntoSight(MyBox(0,0,m_imgSrc.cols-1,m_imgSrc.rows-1)) )
					{
						m_selectedGtIt->box = box;
						m_imgSrc.copyTo( m_imgDraw );
						DrawGt(m_imgDraw);
						DrawGt(m_imgDraw, *m_selectedGtIt);
						imshow( "image", m_imgDraw );
						waitKey( 10 );
					}
				}
				else
				{
					MyBox box = m_boxDraw;
					box.Resize( 1.0f + 1.0f/MIN(32,MIN(box.Height()-1,box.Width()-1)) );
					if( box.MoveIntoSight(MyBox(0,0,m_imgSrc.cols-1,m_imgSrc.rows-1)) )
					{
						m_boxDraw = box;
						m_imgSrc.copyTo( m_imgDraw );
						DrawGt(m_imgDraw);
						rectangle(m_imgDraw, Point(box.m_xmin, box.m_ymin), Point(box.m_xmax, box.m_ymax), color);
						imshow( "image", m_imgDraw );
						waitKey( 10 );
					}
				}
			}
			else if( 'v' == key )
			{
				list<Gt> &gtListCurrFrm = m_gtXml.m_gtList;
				list<Gt>::iterator it;
				for(it = m_gtListLastFrm.begin(); it != m_gtListLastFrm.end(); it++)
				{
					MyBox &box = it->box;
					if((box.m_xmax < m_imgSrc.cols) && (box.m_ymax < m_imgSrc.rows))
					{
						gtListCurrFrm.push_back(*it);
					}
				}
				m_imgSrc.copyTo( m_imgDraw );
				DrawGt(m_imgDraw);
				imshow( "image", m_imgDraw );
				waitKey( 10 );
			}
			else if( 't' == key )
			{
				if(m_bSelected)
				{
					InsertName(*m_selectedGtIt);

					m_imgSrc.copyTo( m_imgDraw );
					DrawGt(m_imgDraw);
					imshow( "image", m_imgDraw );
					waitKey( 10 );
					m_bSelected = false;
				}
				else
				{
					cout<<"select a box with right button first!"<<endl;
				}
			}
			else if( 'g' == key )
			{
				if(m_bSelected)
				{
					InsertNameFromInput(*m_selectedGtIt);

					m_imgSrc.copyTo( m_imgDraw );
					DrawGt(m_imgDraw);
					imshow( "image", m_imgDraw );
					waitKey( 10 );
					m_bSelected = false;
				}
				else
				{
					cout<<"select a box with right button first!"<<endl;
				}
			}
			else if( 'a' == key )
			{
				if(m_bSelected)
				{
					MyBox &box = m_selectedGtIt->box;
					int dx = MIN(box.m_xmin, MAX(1,box.Width()/32));
					box.Add(MyBox(-dx,0,-dx,0));
					m_imgSrc.copyTo( m_imgDraw );
					DrawGt(m_imgDraw);
					DrawGt(m_imgDraw, *m_selectedGtIt);
					imshow( "image", m_imgDraw );
					waitKey( 10 );
				}
			}
			else if( 'd' == key )
			{
				if(m_bSelected)
				{
					MyBox &box = m_selectedGtIt->box;
					int dx = MIN(m_imgSrc.cols-1-box.m_xmax, MAX(1,box.Width()/32));
					box.Add(MyBox(dx,0,dx,0));
					m_imgSrc.copyTo( m_imgDraw );
					DrawGt(m_imgDraw);
					DrawGt(m_imgDraw, *m_selectedGtIt);
					imshow( "image", m_imgDraw );
					waitKey( 10 );
				}
			}
			else if( 'w' == key )
			{
				if(m_bSelected)
				{
					MyBox &box = m_selectedGtIt->box;
					int dy = MIN(box.m_ymin, MAX(1,box.Height()/32));
					box.Add(MyBox(0,-dy,0,-dy));
					m_imgSrc.copyTo( m_imgDraw );
					DrawGt(m_imgDraw);
					DrawGt(m_imgDraw, *m_selectedGtIt);
					imshow( "image", m_imgDraw );
					waitKey( 10 );
				}
			}
			else if( 's' == key )
			{
				if(m_bSelected)
				{
					MyBox &box = m_selectedGtIt->box;
					int dy = MIN(m_imgSrc.rows-1-box.m_ymax, MAX(1,box.Height()/32));
					box.Add(MyBox(0,dy,0,dy));
					m_imgSrc.copyTo( m_imgDraw );
					DrawGt(m_imgDraw);
					DrawGt(m_imgDraw, *m_selectedGtIt);
					imshow( "image", m_imgDraw );
					waitKey( 10 );
				}
			}
			else if( 'b' == key )
			{
				imgidx = MAX(-1, imgidx-2);
				break;
			}
			else if( 'c' == key )
			{
				if(m_bSelected)
				{
					InsertAttrs(*m_selectedGtIt);
				}
			}
			else if( 'q' == key )
			{
				m_tipId = (m_tipId+1) % m_attributes.size();
				m_imgSrc.copyTo( m_imgDraw );
				DrawGt(m_imgDraw);
				imshow( "image", m_imgDraw );
				waitKey( 10 );
			}
			else if( 'p' == key )
			{
				m_defaultNameId = (m_defaultNameId+1) % m_names.size();
				cout<<"default name -> "<<m_names[m_defaultNameId]<<endl;
			}
		}
	}
}

void MySampler::DrawGt(Mat &img)
{
	list<Gt>::iterator it;
	list<Gt> &gtList = m_gtXml.m_gtList; 
	for(it = gtList.begin(); it != gtList.end(); it++)
	{
		MyBox &box = it->box;
		map<string, Scalar>::iterator colorIt = m_nameColors.find(it->name);
		Scalar &color = (m_nameColors.end() != colorIt) ? colorIt->second : m_nameColors.find("NULL")->second;
		rectangle(img, Point(box.m_xmin,box.m_ymin), Point(box.m_xmax,box.m_ymax), color);
		if(0 == m_tipId)
		{
			putText(img, it->name, Point(box.m_xmin,box.m_ymin+12), CV_FONT_HERSHEY_COMPLEX, 0.5, color, 1);
		}
		else
		{
			Scalar &color = m_attrColors.find(m_attributes[m_tipId-1])->second;
			map<string,string>::iterator attrIt = it->attrs.find(m_attributes[m_tipId-1]);
			string &text = (it->attrs.end() != attrIt) ? attrIt->second : string();
			putText(img, m_attributes[m_tipId-1]+":"+text, Point(box.m_xmin,box.m_ymin+12), CV_FONT_HERSHEY_COMPLEX, 0.5, color, 1);
		}
	}
}

void MySampler::DrawGt(Mat &img, Gt &gt)
{
	MyBox &box = gt.box;
	map<string, Scalar>::iterator colorIt = m_nameColors.find(gt.name);
	Scalar &color = (m_nameColors.end() != colorIt) ? colorIt->second : m_nameColors.find("NULL")->second;
	rectangle(img, Point(box.m_xmin,box.m_ymin), Point(box.m_xmax,box.m_ymax), color, 2);
}

void MySampler::PrintTips(vector<string> &strs)
{
	int k;
	for(k = 0; k < (int)strs.size(); k++)
	{
		cout<<k<<" - "<<strs[k]<<", ";
	}
	cout<<"\b\b "<<endl;
}

void MySampler::InsertName(Gt &gt)
{
	PrintTips(m_names);

	int idx = -1;
	string str;
	getline(cin, str, '\n');
	sscanf_s(str.c_str(), "%d", &idx);
	
	if((idx >= 0) && (idx < (int)m_names.size()))
	{
		gt.name = m_names[idx];
	}
}

void MySampler::InsertNameFromInput(Gt &gt)
{
	string str;
	cout<<"input the attribution: ";
	getline(cin, str, '\n');
	gt.name = str;
}

void MySampler::InsertAttrs(Gt &gt)
{
	string temp;
	vector<string> subtemp;
	map<string,string> &attrs = gt.attrs;
	int idx = -1;

	PrintTips(m_attributes);
	getline(cin, temp, '\n');

	StringSplit(temp, subtemp);
	if(0 == subtemp.size())
	{
		return;
	}
	else if(1 == subtemp.size())//只输入属性名，则删除该属性
	{
		sscanf_s(subtemp[0].c_str(), "%d", &idx);
		map<string,string>::iterator it = attrs.find(m_attributes[idx]);
		if(it != attrs.end())
		{
			attrs.erase(it);
		}
	}
	else
	{
		sscanf_s(subtemp[0].c_str(), "%d", &idx);
		if((idx >= 0) && (idx < (int)m_attributes.size()))
		{
			map<string,string>::iterator it = attrs.find(m_attributes[idx]);
			if(it != attrs.end())
			{
				it->second = subtemp[1];
			}
			else
			{
				attrs.insert(pair<string,string>(m_attributes[idx],subtemp[1]));
			}
		}
	}
}

