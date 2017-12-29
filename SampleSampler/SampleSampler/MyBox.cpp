#include "MyBox.h"


MyBox::MyBox(void)
{
}


MyBox::~MyBox(void)
{
}

void MyBox::Resize(float f)
{
	int w0 = Width();
	int h0 = Height();
	int w1 = (int)(w0 * f + 0.5f);
	int h1 = (int)(h0 * f + 0.5f);
	int dx = (w1 - w0) >> 1;
	int dy = (h1 - h0) >> 1;
	m_xmin = m_xmin - dx;
	m_ymin = m_ymin - dy;
	m_xmax = m_xmin + w1 - 1;
	m_ymax = m_ymin + h1 - 1;
}

void MyBox::Shrink(float ratioWpH)
{
	int w = m_xmax - m_xmin + 1;
	int h = m_ymax - m_ymin + 1;
	float ratio = (float)w / (float)h;
	if(ratio >= ratioWpH)
	{
		int dx = w - (int)(h * ratioWpH + 0.5f);
		m_xmin += dx >> 1;
		m_xmax -= dx - (dx >> 1);
	}
	else
	{
		int dy = h - (int)(w / ratioWpH + 0.5f);
		m_ymin += dy >> 1;
		m_ymax -= dy - (dy >> 1);
	}
}

bool MyBox::MoveIntoSight(MyBox &sight)
{
	int h = Height();
	int w = Width();
	if( (h>sight.Height()) || (w>sight.Width()) )
	{
		return false;
	}

	m_xmin = MAX(sight.m_xmin, m_xmin);
	m_ymin = MAX(sight.m_ymin, m_ymin);
	m_xmax = m_xmin + w - 1;
	m_ymax = m_ymin + h - 1;

	m_xmax = MIN(m_xmax, sight.m_xmax);
	m_ymax = MIN(m_ymax, sight.m_ymax);
	m_xmin = m_xmax - w + 1;
	m_ymin = m_ymax - h + 1;

	return true;
}

