#ifndef __FONTCONTROL_H
#define __FONTCONTROL_H

#include <qfontdatabase.h>
#include <qfontmetrics.h>
#include "StyleConsts.h"

class FontControl
{
    int * m_fontsizes;
    int m_size, g_size;
    QString m_fontname;
    QString m_fixedfontname;
    int m_maxsize;
    bool m_hasCourier;
    int m_leading, m_extraspace;
    unsigned char m_basesize;
 public:
    void setBaseSize(unsigned char _s) { m_basesize = _s; }
    unsigned char getBaseSize() { return m_basesize; }
    int gzoom();
    FontControl(QString n = "helvetica", int size = 10)
	:
	m_fontsizes(NULL), m_hasCourier(false), m_leading(0), m_extraspace(0)
    {
	ChangeFont(n, size);
    }
    ~FontControl()
	{
	    if (m_fontsizes != NULL) delete [] m_fontsizes;
	}
    void hasCourier(bool _b, const QString& _nm)
	{
	    m_hasCourier = _b;
	    m_fixedfontname = _nm;
	}
    QString& fixedfontname() { return m_fixedfontname; }
    bool hasCourier() { return m_hasCourier; }
    QString name() { return m_fontname; }
    int currentsize() { return m_fontsizes[m_size]; }
    int getsize(const CStyle& size)
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
    int ascent(const CStyle& ch)
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
    int descent(const CStyle& ch)
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
    int lineSpacing(const CStyle& ch)
    {
	QFont f(name(), getsize(ch));
	QFontMetrics fm(f);
	return fm.lineSpacing();
    }
    bool decreasesize()
	{
/*
	    if (--m_size < 0)
	    {
		m_size = 0;
		return false;
	    }
	    else return true;
*/
	    if (g_size-- == m_size)
	    {
		if (--m_size < 0)
		{
		    m_size = 0;
		}
	    }
//	    qDebug("Font:%d Graphics:%d", m_size, g_size);
	    return true;
	}
    bool increasesize()
	{
/*
	    if (++m_size >= m_maxsize)
	    {
		m_size = m_maxsize - 1;
		return false;
	    }
	    else return true;
*/
	    if (g_size++ == m_size)
	    {
		if (++m_size >= m_maxsize)
		{
		    m_size = m_maxsize - 1;
		}
	    }
//	    qDebug("Font:%d Graphics:%d", m_size, g_size);
	    return true;
	}
    bool ChangeFont(QString& n)
	{
	    return ChangeFont(n, currentsize());
	}
    bool ChangeFont(QString& n, int tgt);
    void setlead(int _lead)
	{
	    m_leading = _lead;
	}
    int getlead()
	{
	    return m_leading;
	}
    void setextraspace(int _lead)
	{
	    m_extraspace = _lead;
	}
    int getextraspace()
	{
	    return m_extraspace;
	}
};

#endif
