#ifndef VUMETER_H
#define VUMETER_H

#include "qtrec.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>

typedef struct {
    bool onOff;
    int hold,update,leds,reso;
    float resoFactor;
} VUParameters;

class VUMeter: public QWidget
{
  Q_OBJECT

public:
    VUMeter(QWidget *parent=0, const char *name=0, const int tracks=2);
    ~VUMeter();
    VUParameters para;
    QTimer *vuTimer;
    void startTimer();
    void stopTimer();
public slots:
    void setTracks(int trk) {tracks=trk;};
    void setPeak(int a[]);
    void setPeak(float a[]);
    void timeSlot();
    void slotOn();
    void slotOff();
    void update();
    void slotProps();
    void resize();
    void readConf();
    void writeConf();

signals:
    void statusChanged(const char *);

protected:

    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);
    virtual void mousePressEvent(QMouseEvent*);
    void paint();
private:
    int tracks, colorScheme;
    float resoFactor;
    int x,y,sx,sy,dx,dy,ox,oy;
    float peak[4], hold[4];
    int i_peak[4], i_hold[4];
    int holdTime[4];

    QPixmap *buffer, *warning;
    QPopupMenu *vuProp;
    QColor color[37];
};


#endif
