#ifndef TONLEITER_FRET_BOARD_H
#define TONLEITER_FRET_BOARD_H

#include <qbrush.h>
#include <qpen.h>
#include <qvaluelist.h>
#include <qwidget.h>


#include "tonleiterdata.h"

class FretBoard : public QWidget
{
    Q_OBJECT
private:
    TonleiterData* data;

    QPen fretpen;
    QBrush markerbrush;
    QPen stringpen;

    Instrument inst;

    int xmargin;
    int xmin;
    int xmax;
    int xwidth;

    int ymargin;
    int ymin;
    int ymax;
    int yheight;

    double fretdist;
    double stringdist;

public:
    FretBoard(TonleiterData* data,QWidget* parent,const char* name="FretBoard",WFlags f=0);
    virtual ~FretBoard();
private slots:
    void dataChange();
private:
    void paintEvent(QPaintEvent* pe);
    void resizeEvent(QResizeEvent* re);
    //void mousePressEvent(QMouseEvent* mpe);
    //void mouseMoveEvent(QMouseEvent* mme);
    //void mouseReleaseEvent(QMouseEvent* mre);
    void paintBoard(QPainter* p);
    void paintFrets(QPainter* p);
    void paintStrings(QPainter* p);
    void paintScale(QPainter* p);
};

#endif //TONLEITER_FRET_BOARD_H
