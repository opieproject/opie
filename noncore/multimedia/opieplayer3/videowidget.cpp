#include "videowidget.h"
#include "../opieplayer2/lib.h"
#include "../opieplayer2/threadutil.h"
#include "../opieplayer2/xinevideowidget.h"

#include <opie2/odebug.h>

#include <qpe/qpeapplication.h>

#include <qlayout.h>
#include <qslider.h>
#include <qlabel.h>

VideoWidget::VideoWidget( QWidget * parent, const char * name, WFlags f)
    :QWidget(parent,name,f)
{
    m_xineLib = 0;

    m_MainLayout = new QVBoxLayout(this);
    m_MainLayout->setAutoAdd(true);
    m_Videodisplay = new XineVideoWidget(this,"videodisp");
    m_PosSlider = new QSlider(QSlider::Horizontal,this);
    m_PosSlider->setTickInterval(60);
    connect(m_PosSlider,SIGNAL(valueChanged(int)),this,SLOT(slotNewPos(int)));
    connect(m_PosSlider,SIGNAL(sliderMoved(int)),this,SLOT(slotNewPos(int)));
    connect(m_PosSlider,SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
    connect(m_PosSlider,SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));
    connect(m_Videodisplay,SIGNAL(videoResized ( const QSize & )),this,SLOT(slot_Videoresized(const QSize&)));
    connect(m_Videodisplay,SIGNAL(clicked()),this,SLOT(slotClicked()));
    m_pressed = false;
    m_uppos=0;
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::slotClicked()
{
    odebug << "clicked " << oendl;
    emit videoclicked();
}

void VideoWidget::closeEvent(QCloseEvent*e)
{
}

void VideoWidget::slotNewPos(int pos)
{
    if (!m_xineLib) return;
    if (m_uppos==pos) return;
    m_xineLib->seekTo(pos);
}

void VideoWidget::sliderPressed()
{
    m_pressed = true;
}

void VideoWidget::sliderReleased()
{
    m_pressed = false;
}

void VideoWidget::fullScreen(bool how)
{
    if (how) {
        m_PosSlider->hide();
    } else {
        m_PosSlider->show();
    }
}

void VideoWidget::playFile(const DocLnk&aLnk,XINE::Lib*aLib)
{
    m_current = aLnk;
    if (m_xineLib != aLib) {
        disconnect(m_xineLib);
        m_xineLib = aLib;
        if (aLib) m_xineLib->setWidget(m_Videodisplay);
    }
    if (!m_xineLib) {
        return;
    }
    connect(m_xineLib,SIGNAL(stopped()),this,SLOT(slotStopped()));
    m_uppos=0;
    m_PosSlider->setValue(0);
    m_xineLib->setWidget(m_Videodisplay);
    m_xineLib->setShowVideo(true);
    m_xineLib->resize(m_Videodisplay->size());
    m_xineLib->play(m_current.file());
    int l = m_xineLib->length();
    m_PosSlider->setRange(0,l);
    m_PosSlider->setPageStep(l/10);
}

void VideoWidget::stopPlaying()
{
    if (m_xineLib) {
        m_xineLib->stop();
    }
}

void VideoWidget::slotStopped()
{
// check fullscreen here!
}

void VideoWidget::slot_Videoresized(const QSize&s)
{
    odebug << "Videoresized: " << s << oendl;
    if (m_xineLib) {
        m_xineLib->resize(s);
    }
}

void VideoWidget::updatePos(int val)
{
    if (m_pressed) return;
    m_uppos = val;
    m_PosSlider->setValue(val);
}
