#ifndef TONLEITER_PIANO_SCALE_H
#define TONLEITER_PIANO_SCALE_H

#include <qwidget.h>
#include <qpen.h>
#include <qbrush.h>

namespace Menu
{
    class PianoScale : public QWidget
    {
        Q_OBJECT
    private:
        QBrush blackBrush;
        QBrush whiteBrush;
        QBrush markBrush;
        QPen blackPen;
        QPen whitePen;
    public:
        PianoScale(QWidget* parent,const char* name="PianoScale",WFlags f=0);
        ~PianoScale();
    private:
        void paintEvent(QPaintEvent* pe);
        void mousePressEvent(QMouseEvent* me);
    };
};

#endif //TONLEITER_PIANO_SCALE_H
