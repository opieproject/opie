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
    bool m_hasCourier;
 public:
    FontControl(QString n = "helvetica", int size = 10)
	:
	m_fontsizes(NULL), m_hasCourier(false)
    {
	ChangeFont(n, size);
    }
    ~FontControl()
	{
	    if (m_fontsizes != NULL) delete [] m_fontsizes;
	}
    void hasCourier(bool _b) { m_hasCourier = _b; }
    bool hasCourier() { return m_hasCourier; }
    QString name() { return m_fontname; }
    int currentsize() { return m_fontsizes[m_size]; }
    int getsize(CStyle size)
	{
	    int tgt = m_size+size.getFontSize();
	    if (tgt < 0)
	    {
		tgt = 0;
	    }
	    if (tgt >= m_maxsize)
	    {
		tgt = m_maxsize - 1;
	    }
	    return m_fontsizes[tgt];
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
    bool ChangeFont(QString& n, int tgt);
};

#endif
