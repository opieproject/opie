/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <qwidget.h>
#include <qvbox.h>

class MFrequencySpectrum : public QWidget
{
  public:
    MFrequencySpectrum( int channels, QWidget* parent = 0, const char* name = "MFrequencySpectrum", WFlags f = 0 );
    int value( int channel ) const { return _values[channel]; };
    void setValue( int channel, int value )
    {
        if ( value > _values[channel] )
        {
            _values[channel] = value;
            _dirty[channel] = true;
        }
    };
    void decrease( int channel, int amount )
    {
        if ( _values[channel] >= amount )
        {
            _values[channel] -= amount;
            _dirty[channel] = true;
        }
    };

  protected:
    virtual void paintEvent( QPaintEvent* );
    virtual void mousePressEvent( QMouseEvent* );

    void drawLine( QPainter* p, int x, int y, int width, const QColor& c );
    void drawTopLine( QPainter* p, int x, int y, int width, const QColor& c );
    void drawBottomLine( QPainter* p, int x, int y, int width, const QColor& c );
    void MFrequencySpectrum::drawBar( QPainter* p, int x, int y, int width, int height, int maxheight );

  private:
    int _channels;
    int* _values;
    bool* _dirty;
};


class Legende : public QFrame
{
  public:
    Legende( int channels, QWidget* parent = 0, const char* name = "Legende", WFlags f = 0 );

  protected:
    virtual void drawContents( QPainter* );

  private:
    int _channels;
};


class MGraphWindow : public QVBox
{
  Q_OBJECT

  public:
    MGraphWindow( QWidget* parent = 0, const char* name = "MGraphWindow", WFlags f = 0 );
    void traffic( int channel, int signal );

  protected:
    virtual void timerEvent( QTimerEvent* e );

  protected slots:
    virtual void testGraph();

  protected:
    MFrequencySpectrum* spectrum;
    Legende* legende;

};

/* XPM */
static char * background[] = {
"16 16 6 1",
" 	c None",
".	c #52676E",
"+	c #3F545B",
"@	c #394E56",
"#	c #2F454C",
"$	c #364B52",
".+++++++++++++++",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############",
"+$$$$$$$$$$$$$$$",
"@###############"};


#endif

