#include "audiowidget.h"
#include "../opieplayer2/lib.h"
#include "../opieplayer2/threadutil.h"

#include <opie2/odebug.h>

#include <qlayout.h>
#include <qtextview.h>
#include <qslider.h>
#include <qlabel.h>

AudioWidget::AudioWidget( QWidget * parent, const char * name, WFlags f)
    :QWidget(parent,name,f)
{
    m_xineLib = 0;

    m_MainLayout = new QVBoxLayout(this);
    m_MainLayout->setAutoAdd(true);
    m_InfoBox = new QTextView(this);
    m_PosSlider = new QSlider(QSlider::Horizontal,this);
    m_PosSlider->setTickInterval(60);
    connect(m_PosSlider,SIGNAL(valueChanged(int)),this,SLOT(slotNewPos(int)));
    connect(m_PosSlider,SIGNAL(sliderMoved(int)),this,SLOT(slotNewPos(int)));
    connect(m_PosSlider,SIGNAL(sliderPressed()),this,SLOT(sliderPressed()));
    connect(m_PosSlider,SIGNAL(sliderReleased()),this,SLOT(sliderReleased()));
    m_pressed = false;
    m_uppos=0;
}

AudioWidget::~AudioWidget()
{
}

void AudioWidget::slotNewPos(int pos)
{
    if (!m_xineLib) return;
    if (m_uppos==pos) return;
    m_xineLib->seekTo(pos);
}

void AudioWidget::sliderPressed()
{
    m_pressed = true;
}

void AudioWidget::sliderReleased()
{
    m_pressed = false;
}

void AudioWidget::closeEvent(QCloseEvent*e)
{
    odebug << "AudioWidget::closeEvent(QCloseEvent*e)" << oendl;
    if (m_xineLib) {
        m_xineLib->stop();
    }
    QWidget::closeEvent(e);
}

void AudioWidget::playFile(const DocLnk&aLnk,XINE::Lib*aLib)
{
    m_current = aLnk;
    if (m_xineLib != aLib) {
        disconnect(m_xineLib);
        m_xineLib = aLib;
    }
    if (!m_xineLib) {
        return;
    }
    m_uppos=0;
    m_PosSlider->setValue(0);
    m_xineLib->setShowVideo(false);
    m_xineLib->play(m_current.file());
    // title
    QString title = m_xineLib->metaInfo(0);
    // artist
    QString artist = m_xineLib->metaInfo(2);
    // album
    QString album = m_xineLib->metaInfo(4);

    int l = m_xineLib->length();
    m_PosSlider->setRange(0,l);
    QString laenge="";
    int h = l/3600;
    l-=h*3600;
    int m = l/60;
    l-=m*60;
    if (h>0) {
        laenge+=QString("%1 h").arg(h);
    }
    if (m>0) {
        if (!laenge.isEmpty()) laenge+=" ";
        laenge+=QString("%1 m").arg(m);
    }
    if (l>0) {
        if (!laenge.isEmpty()) laenge+=" ";
        laenge+=QString("%1 s").arg(l);
    }
    QString text = "<qt>";
    if (artist.length()) {
        text+="<H2><center>"+artist+"</center></h2>";
    }
    if (title.length()) {
        text+="<H2><center>"+title+"</center></h2>";
    }
    if (album.length()) {
        text+="<H2><center>"+album+"</center></h2>";
    }
    text+="<h3><center>"+laenge+"</center></h3>";
    m_InfoBox->setText(text);
}

void AudioWidget::stopPlaying()
{
    if (m_xineLib) {
        m_xineLib->stop();
    }
}

void AudioWidget::updatePos(int val)
{
    if (m_pressed) return;
    m_uppos = val;
    m_PosSlider->setValue(val);
}
