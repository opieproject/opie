#ifndef __videowidget_h
#define __videowidget_h
#include <qwidget.h>

#include <qpe/applnk.h>

namespace XINE {
    class Lib;
}

class QVBoxLayout;
class QSlider;
class QLabel;
class XineVideoWidget;

class VideoWidget:public QWidget
{
    Q_OBJECT
public:
    VideoWidget( QWidget * parent=0, const char * name=0, WFlags f=0 );
    virtual ~VideoWidget();

    void playFile(const DocLnk&,XINE::Lib*);
    void fullScreen(bool how);

signals:
    void videoclicked();

public slots:
    virtual void stopPlaying();
    virtual void updatePos(int);

protected:
    XINE::Lib*m_xineLib;
    DocLnk m_current;
    QVBoxLayout*m_MainLayout;
    XineVideoWidget*m_Videodisplay;
    QWidget * m_holder;
    QSlider*m_PosSlider;
    bool m_pressed:1;
    int m_uppos;

protected slots:
    virtual void closeEvent(QCloseEvent*e);
    virtual void slotNewPos(int pos);
    virtual void sliderPressed();
    virtual void sliderReleased();
    virtual void slot_Videoresized(const QSize&);
    virtual void slotStopped();
    virtual void slotClicked();
};

#endif
