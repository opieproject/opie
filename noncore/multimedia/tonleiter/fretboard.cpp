#include "fretboard.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qpainter.h>

Graph::FretBoard::FretBoard(TonleiterData* data,QWidget* parent,const char* name,WFlags f)
:QWidget(parent,name,f),data(data)
{
    setBackgroundColor(QColor(0,0,0));
    fretpen=QPen(QColor(155,155,155),0);
    markerbrush=QBrush(QColor(155,155,155));
    stringpen=QPen(QColor(255,255,255),3);
    connect(data,SIGNAL(dataChange()),this,SLOT(dataChange()));
}
//****************************************************************************
Graph::FretBoard::~FretBoard()
{
}
//****************************************************************************
void Graph::FretBoard::dataChange()
{
    repaint(true);
}
//****************************************************************************
void Graph::FretBoard::paintEvent(QPaintEvent* pe)
{
    Q_UNUSED(pe);

    int instid=data->getCurrentInstrumentID();
    inst=data->getInstrument(instid);
    //odebug << "inst " << instid << "  is " << inst.instName().data() << "" << oendl; 

    QRect mysize=rect();

    xmargin=(int)(mysize.width()*0.05);
    xmin=2*xmargin;
    xmax=mysize.width()-xmargin;
    xwidth=xmax-xmin;

    ymargin=(int)(mysize.height()*0.1);
    ymin=ymargin;
    ymax=mysize.height()-2*ymargin;
    yheight=ymax-ymin;

    QPainter p(this);
    paintBoard(&p);
    paintFrets(&p);
    paintStrings(&p);
    paintScale(&p);
}
//****************************************************************************
void Graph::FretBoard::resizeEvent(QResizeEvent* re)
{
    Q_UNUSED(re);
}
//****************************************************************************
void Graph::FretBoard::mouseReleaseEvent(QMouseEvent* me)
{
    Q_UNUSED(me);
    emit pressed();
}
//****************************************************************************
void Graph::FretBoard::paintBoard(QPainter* p)
{
    //debug
    QColor bgc=QColor(142,138,120);
    p->setPen(QPen(bgc,5));
    p->setBrush(QBrush(bgc));
    p->drawRect(xmin,ymin,xwidth,yheight);

}
//****************************************************************************
void Graph::FretBoard::paintFrets(QPainter* p)
{
    //draw frets
    p->setPen(fretpen);
    p->setBrush(markerbrush);
    fretdist=(double)(xwidth)/(double)(inst.noOfFrets());

    int markerwidth = (int)(fretdist/4.0);
    if(markerwidth>5)
        markerwidth=5;
    else if(markerwidth<2)
        markerwidth=2;

    for(int f=0;f<=inst.noOfFrets();f++)
    {
        int fretpos=(int)(fretdist*f);
        p->drawLine(fretpos+xmin,ymin,fretpos+xmin,ymax);

        int above_octave=f%12;
        int octave=(f-above_octave)/12;
        int marker=f-12*octave;


        if(marker==5 || marker==7 || marker==9)
        {
            p->drawEllipse(fretpos+xmin,ymax+ymargin,markerwidth,markerwidth);
        }
        else if(marker==0 && f!=0)
        {
            p->drawEllipse(fretpos+xmin,ymax+ymargin+2*markerwidth,markerwidth,markerwidth);
            p->drawEllipse(fretpos+xmin,ymax+ymargin-2*markerwidth,markerwidth,markerwidth);
        }

    }
}
//****************************************************************************
void Graph::FretBoard::paintStrings(QPainter* p)
{
    //draw strings
    p->setPen(stringpen);

    stringdist=(double)(yheight)/(double)(inst.noOfStrings()-1);
    for(int s=0;s<inst.noOfStrings();s++)
    {
        int stringpos=(int)(stringdist*s);
        p->drawLine(xmin,stringpos+ymin,xmax,stringpos+ymin);

        int stinglabelid=inst.noOfStrings()-(s+1);
        QString label=Note::getNameOfNote(inst.string(stinglabelid));
        p->drawText(xmargin/2,stringpos+ymin,label);
    }
}
//****************************************************************************
void Graph::FretBoard::paintScale(QPainter* p)
{
    int dotsize=10;
    int scaleid=data->getCurrentScaleID();
    Scale scale=data->getScale(scaleid);
    int baseoctave=Note::octaveOfBaseNote(data->getCurrentBaseNote(),inst.lowestNote());

    for(int s=0;s<inst.noOfStrings();s++)
    {
        int y=(int)(stringdist*s)+ymin;
        int stingid=inst.noOfStrings()-(s+1);
        for(int f=0;f<=inst.noOfFrets();f++)
        {
            int note=inst.string(stingid)+f;
            if(scale.noteInScale(data->getCurrentBaseNote(),note))
            {
                int x=(int)(fretdist*f)+xmin;

                //no more than six octaves can be visualised (there is a zero octave)
                int octave=Note::octaveOfBaseNote(data->getCurrentBaseNote(),note)-baseoctave;
                if(octave<0)
                    odebug << "" << octave << "," << baseoctave << "" << oendl; 
                if(octave>5)
                {
                    odebug << "octave out of range" << oendl; 
                    octave=5;
                }

                p->setPen(QColor(255,0,0));
                int c= ( (note-12*baseoctave) - (12*octave+data->getCurrentBaseNote()) )*15;
                if(c<0 || c>255)
                    odebug << "" << c << " = " << note << " - ( " << 12*octave << " + " << data->getCurrentBaseNote() << ")" << oendl; 
                QColor dotcolor(255,255,255);

                if(octave==0)
                    dotcolor=QColor(c,c,255);
                else if(octave==1)
                    dotcolor=QColor(c,255,c);
                else if(octave==2)
                    dotcolor=QColor(255,c,c);
                else if(octave==3)
                    dotcolor=QColor(255,255,c);
                else if(octave==4)
                    dotcolor=QColor(255,c,255);
                else
                    dotcolor=QColor(c,255,255);

                p->setBrush(dotcolor);
                p->drawEllipse(x-dotsize/2,y-dotsize/2,dotsize,dotsize);

                if(data->isDrawNames())
                {
                    p->setPen(QColor(255,155,0));
                    p->drawText(x-dotsize/2,y-dotsize/2,Note::getNameOfNote(note));
                }

            }
        }
    }
}
//****************************************************************************
//****************************************************************************
