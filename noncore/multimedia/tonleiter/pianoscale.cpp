#include "pianoscale.h"

#include <qpainter.h>


Menu::PianoScale::PianoScale(QWidget* parent,const char* name,WFlags f)
:QWidget(parent,name,f)
{
    QColor black(0,0,0);
    QColor white(255,255,255);
    QColor mark(255,0,0);
    blackBrush=QBrush(black);
    whiteBrush=QBrush(white);
    markBrush=QBrush(mark);
    blackPen=QPen(black);
    whitePen=QPen(white);
    setBackgroundColor(QColor(0,0,255));
}
//****************************************************************************
Menu::PianoScale::~ PianoScale()
{
}
//****************************************************************************
void Menu::PianoScale::paintEvent(QPaintEvent* pe)
{
    QPainter p(this);
    QRect mysize=rect();

    int pad=10;
    int x0=pad;
    int y0=pad;
    int w0=mysize.width()-2*pad;
    int h0=mysize.height()-2*pad;

    int keypad=2;
    if(mysize.width()>mysize.height())
    {
        int div=(int)(w0/14.0);
        int halftonewidth=(int)(div/3.0);
        int halftoneheight=(int)((h0-2*keypad)*0.66);
        for(int a=0;a<14;a++)
        {
            int x=x0+a*div;

            p.setPen(blackPen);
            p.setBrush(blackBrush);
            p.drawRect(x,y0,div,h0);

            p.setPen(whitePen);
            p.setBrush(whiteBrush);
            p.drawRect(x+keypad,y0+keypad,div-2*keypad,h0-2*keypad);

            if(a==1 || a==2 || a==4 || a==5 || a==6 || a==8 || a==9 || a==11 || a==12 || a==13)
            {
                p.setPen(blackPen);
                p.setBrush(blackBrush);
                p.drawRect(x-halftonewidth,y0+keypad,2*halftonewidth,halftoneheight);
            }
        }
    }
    else
    {
        int div=(int)(w0/7.0);
        int halfheight=(int)(h0/2.0);
        int halftonewidth=(int)(div/3.0);
        int halftoneheight=(int)((halfheight-2*keypad)*0.66);
        for(int a=0;a<7;a++)
        {
            int x=x0+a*div;

            p.setPen(blackPen);
            p.setBrush(blackBrush);
            p.drawRect(x,y0,div,h0);

            p.setPen(whitePen);
            p.setBrush(whiteBrush);
            p.drawRect(x+keypad,y0+keypad,div-2*keypad,halfheight-2*keypad);
            p.drawRect(x+keypad,y0+keypad+halfheight,div-2*keypad,halfheight-2*keypad);

            if(a==1 || a==2 || a==4 || a==5 || a==6)
            {
                p.setPen(blackPen);
                p.setBrush(blackBrush);
                p.drawRect(x-halftonewidth,y0+keypad,2*halftonewidth,halftoneheight);
                p.drawRect(x-halftonewidth,y0+keypad+halfheight,2*halftonewidth,halftoneheight);
            }
        }
    }
}
//****************************************************************************
void Menu::PianoScale::mousePressEvent(QMouseEvent* me)
{
}
//****************************************************************************
//****************************************************************************

