#ifndef _audiowidget_h
#define _audiowidget_h

#include <qwidget.h>

#include <qpe/applnk.h>

namespace XINE {
    class Lib;
}

class QVBoxLayout;
class QTextView;
class QSlider;
class QLabel;

class AudioWidget:public QWidget
{
    Q_OBJECT
public:
    AudioWidget( QWidget * parent=0, const char * name=0, WFlags f=0 );
    virtual ~AudioWidget();

    void playFile(const DocLnk&,XINE::Lib*);

public slots:
    virtual void stopPlaying();
    virtual void updatePos(int);

protected:
    XINE::Lib*m_xineLib;
    DocLnk m_current;
    QVBoxLayout*m_MainLayout;
    QTextView*m_InfoBox;
    QSlider*m_PosSlider;
    bool m_pressed;
    int m_uppos;

protected slots:
    virtual void closeEvent(QCloseEvent*e);
    virtual void slotNewPos(int pos);
    virtual void sliderPressed();
    virtual void sliderReleased();
};

#endif
