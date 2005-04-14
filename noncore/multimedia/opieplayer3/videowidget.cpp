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
    m_scaleUp = false;
    m_MainLayout = new QVBoxLayout(this);
    m_MainLayout->setAutoAdd(true);
    m_Videodisplay = new XineVideoWidget(this,"videodisp");
    connect(m_Videodisplay,SIGNAL(videoResized ( const QSize & )),this,SLOT(slot_Videoresized(const QSize&)));
    connect(m_Videodisplay,SIGNAL(clicked()),this,SLOT(slotClicked()));
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::slotClicked()
{
    emit videoclicked();
}

void VideoWidget::closeEvent(QCloseEvent*e)
{
}

void VideoWidget::fullScreen(bool /* how */)
{
}

int VideoWidget::playFile(const DocLnk&aLnk,XINE::Lib*aLib)
{
    m_current = aLnk;
    bool recon;
    if (m_xineLib != aLib) {
        if (m_xineLib) disconnect(m_xineLib);
        m_xineLib = aLib;
        if (aLib) m_xineLib->setWidget(m_Videodisplay);
    }
    if (!m_xineLib) {
        return -1;
    }
    connect(m_xineLib,SIGNAL(stopped()),this,SLOT(slotStopped()));
    m_xineLib->setWidget(m_Videodisplay);
    m_xineLib->setShowVideo(true);
    int res = m_xineLib->play(m_current.file());
    vSize = m_xineLib->videoSize();

    slot_Videoresized(m_Videodisplay->size());
    odebug << "Xine play: " << res << oendl;
    if (res != 1) {
        return -2;
    }
    return m_xineLib->length();
}

void VideoWidget::stopPlaying()
{
    if (m_xineLib) {
        m_xineLib->stop();
    }
}

void VideoWidget::slotStopped()
{
}

void VideoWidget::slot_Videoresized(const QSize&s)
{
    if (m_xineLib) {
        if (vSize.width()<s.width()&&vSize.height()<s.height()&&!m_scaleUp) {
            m_xineLib->resize(vSize);
        } else {
            m_xineLib->resize(s);
        }
    }
}

void VideoWidget::scaleUp(bool how)
{
    if (how == m_scaleUp) return;
    m_scaleUp = how;
    slot_Videoresized(m_Videodisplay->size());
    m_Videodisplay->repaint();
}

void VideoWidget::updatePos(int /*val*/)
{
}
