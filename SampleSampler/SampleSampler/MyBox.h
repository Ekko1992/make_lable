#pragma once

#include <opencv2\opencv.hpp>
using namespace std;
using namespace cv;

class MyBox
{
public:
	int m_xmin;
	int m_ymin;
	int m_xmax;
	int m_ymax;
public:
	MyBox(void);
	MyBox(int xmin, int ymin, int xmax, int ymax)
	{
		m_xmin = xmin;
		m_ymin = ymin;
		m_xmax = xmax;
		m_ymax = ymax;
	}
	MyBox(const Point &center, const Size &sz)
	{
		m_xmin = center.x - (sz.width>>1);
		m_ymin = center.y - (sz.height>>1);
		m_xmax = m_xmin + sz.width - 1;
		m_ymax = m_ymin + sz.height - 1;
	}
	virtual ~MyBox(void);

	bool Enclose(const Point &pt)
	{
		return ( (pt.x>= m_xmin) && (pt.x <= m_xmax) && (pt.y >= m_ymin) && (pt.y <= m_ymax) );
	}

	int Area()
	{
		return (m_ymax - m_ymin + 1) * (m_xmax - m_xmin + 1);
	}

	int Height()
	{
		return (m_ymax - m_ymin + 1);
	}

	int Width()
	{
		return (m_xmax - m_xmin + 1);
	}

	void Add(MyBox &box)
	{
		m_xmin += box.m_xmin;
		m_ymin += box.m_ymin;
		m_xmax += box.m_xmax;
		m_ymax += box.m_ymax;
	}

	void Resize(float ratio);
	void Shrink(float ratioWpH);
	bool MoveIntoSight(MyBox &sight);	
};

