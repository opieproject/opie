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
}

AudioWidget::~AudioWidget()
{
}

int AudioWidget::playFile(const DocLnk&aLnk,XINE::Lib*aLib)
{
    m_current = aLnk;
    if (m_xineLib != aLib) {
        if (m_xineLib) disconnect(m_xineLib);
        m_xineLib = aLib;
    }
    if (!m_xineLib) {
        return -1;
    }
    int res = m_xineLib->play(m_current.file(),0,0);
    if (res != 1) {
        return -2;
    }
    if (!m_xineLib->hasVideo()) {
        m_xineLib->setShowVideo( false );
    }

    // title
    QString title = m_xineLib->metaInfo(0);
    // artist
    QString artist = m_xineLib->metaInfo(2);
    // album
    QString album = m_xineLib->metaInfo(4);

    int l = m_xineLib->length();
    QString laenge = secToString(l);
    QString text = "<qt><center><table border=\"0\">";
    if (artist.length()) {
        text+="<tr><td>"+tr("Artist: ")+"</td><td><b>"+artist+"</b></td></tr>";
    }
    if (title.length()) {
        text+="<tr><td>"+tr("Title: ")+"</td><td><font size=\"+2\">"+title+"</font></td></tr>";
    } else {
        text+="<tr><td>"+tr("Filename: ")+"</td><td><b>"+m_current.name()+"</b></td></tr>";
    }
    if (album.length()) {
        text+="<tr><td>"+tr("Album: ")+"</td><td><b>"+album+"</b></td></tr>";
    }
    text+="<tr><td>"+tr("Length: ")+"</td><td><b>"+laenge+"</b></td></tr>";
    text+="</table></center></qt>";
    m_InfoBox->setText(text);
    return l;
}

QString AudioWidget::secToString(int sec)
{
    int l = sec;
    int h = l/3600;
    l-=h*3600;
    int m = l/60;
    l-=m*60;
    QString s = "";
    if (h>0) {
        s.sprintf("%2i:%2i:%2i",h,m,l);
    } else {
        s.sprintf("%02i:%02i",m,l);
    }
    return s;
}

void AudioWidget::stopPlaying()
{
    if (m_xineLib) {
        m_xineLib->stop();
    }
}

void AudioWidget::updatePos(int /* val */)
{
}
