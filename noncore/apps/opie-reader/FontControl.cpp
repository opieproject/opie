#include "opie.h"
#include "FontControl.h"

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
#ifdef OPIE
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
	m_size = best;
    }
    return true;
}
