#include "FontControl.h"

int FontControl::gzoom()
{
    int ret;
    if (m_size == g_size)
    {
	ret = m_fontsizes[m_size]*m_basesize;
    }
    else if (g_size < 0)
    {
	int f = -g_size;
	ret = (m_fontsizes[0]*m_basesize) >> (f/2);
	if (f%2) ret = (2*ret/3);
    }
    else
    {
	int f = g_size - m_maxsize + 1;
	ret = (m_fontsizes[m_maxsize-1]*m_basesize) << (f/2);
	if (f%2) ret = (3*ret/2);
    }
    return ret;
}

bool FontControl::ChangeFont(QString& n, int tgt)
{
    QValueList<int>::Iterator it;
    QFontDatabase fdb;
    QValueList<int> sizes = fdb.pointSizes(n);
    if (sizes.count() == 0)
    {
	return false;
    }
    else
    {
	m_fontname = n;
	m_maxsize = sizes.count();
	if (m_fontsizes != NULL) delete [] m_fontsizes;
	m_fontsizes = new int[m_maxsize];
	uint i = 0;
	uint best = 0;
	for (it = sizes.begin(); it != sizes.end(); it++)
	{
#if defined(OPIE) || !defined(USEQPE)
	    m_fontsizes[i] = (*it);
#else
	    m_fontsizes[i] = (*it)/10;
#endif
	    if (abs(tgt-m_fontsizes[i]) < abs(tgt-m_fontsizes[best]))
	    {
		best = i;
	    }
	    i++;
	}
	g_size = m_size = best;
    }
    return true;
}
