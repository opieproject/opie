#ifndef __FONTCONTROL_H
#define __FONTCONTROL_H

#include <qfontdatabase.h>
#include <qfontmetrics.h>
#include "StyleConsts.h"

class FontControl
{
    int * m_fontsizes;
    int m_size;
    QString m_fontname;
    int m_maxsize;
 public:
    FontControl(QString n = "helvetica", int size = 10)
	:
	m_fontsizes(NULL)
    {
	ChangeFont(n, size);
    }
    ~FontControl()
	{
	    if (m_fontsizes != NULL) delete [] m_fontsizes;
	}
    QString name() { return m_fontname; }
    int currentsize() { return m_fontsizes[m_size]; }
    int getsize(CStyle size)
	{
	    return m_fontsizes[m_size+size.getFontSize()];
	}
    int ascent()
    {
	QFont f(name(), currentsize());
	QFontMetrics fm(f);
	return fm.ascent();
    }
    int ascent(CStyle ch)
    {
	QFont f(name(), getsize(ch));
	QFontMetrics fm(f);
	return fm.ascent();
    }
    int descent()
    {
	QFont f(name(), currentsize());
	QFontMetrics fm(f);
	return fm.descent();
    }
    int descent(CStyle ch)
    {
	QFont f(name(), getsize(ch));
	QFontMetrics fm(f);
	return fm.descent();
    }
    int lineSpacing()
    {
	QFont f(name(), currentsize());
	QFontMetrics fm(f);
	return fm.lineSpacing();
    }
    int lineSpacing(CStyle ch)
    {
	QFont f(name(), getsize(ch));
	QFontMetrics fm(f);
	return fm.lineSpacing();
    }
    bool decreasesize()
	{
	    if (--m_size < 0)
	    {
		m_size = 0;
		return false;
	    }
	    else return true;
	}
    bool increasesize()
	{
	    if (++m_size >= m_maxsize)
	    {
		m_size = m_maxsize - 1;
		return false;
	    }
	    else return true;
	}
    bool ChangeFont(QString& n)
	{
	    return ChangeFont(n, currentsize());
	}
    bool ChangeFont(QString& n, int tgt)
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
		    m_fontsizes[i] = (*it)/10;
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
};

#endif
