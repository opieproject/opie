#include "CHM.h"
#include "chm_lib.h"
#include "static.h"
#include <qstring.h>
#include <qstringlist.h>
#include <stdio.h>
#include <qimage.h>
#include <qpixmap.h>

#ifndef __STATIC
extern "C"
{
  CExpander* newcodec() { return new CHM; }
}
#endif

static int _print_ui_index(struct chmFile *h, struct chmUnitInfo *ui,
                        void *context)
{
    CHM *chm = (CHM *)context;
    QString temp = "<tr>";
    char buff[1024];
    sprintf( buff,"<td align=right>%8d\n</td><td><a href=\"%s\">%s</a></td></tr>",(int)ui->length, ui->path, ui->path);
    temp += buff;
    chm->addContent(temp);
    return CHM_ENUMERATOR_CONTINUE;
}

static int _get_hhc (struct chmFile *h, struct chmUnitInfo *ui,
                        void *context)
{
    CHM *chm = (CHM *)context;
    QString PathName = ui->path;
    if (PathName.find(".hhc") > -1) {
        chm->setPath(PathName);
    }
    return CHM_ENUMERATOR_CONTINUE;
}

CHM::CHM() {
    chmFile = NULL;
    chmPath = "";
    chmHHCPath = "";
    chmBuffer = "";
    bufpos = 0;
}

CHM::~CHM()  {
    if (chmFile != NULL)
        chm_close(chmFile);
}

void CHM::suspend() {
#ifdef USEQPE
        bSuspended = true;
        //suspos = gztell(file);
        chm_close(chmFile);
        chmFile = NULL;
        sustime = time(NULL);
#endif
}

void CHM::unsuspend() {
#ifdef USEQPE
        if (bSuspended)
        {
            bSuspended = false;
            int delay = time(NULL) - sustime;
            if (delay < 10)
                sleep(10-delay);
            chmFile = chm_open(fname);
            for (int i = 0; chmFile == NULL && i < 5; i++) {
                sleep(5);
                chmFile = chm_open(fname);
            }
            if (chmFile == NULL) {
                QMessageBox::warning(NULL, PROGNAME, "Couldn't reopen file");
                exit(0);
            }
        //suspos = gzseek(file, suspos, SEEK_SET);
        }
#endif
}

void CHM::addContent(QString content) {
    chmBuffer += content;
}

void CHM::FillHomeContent() {
    if (chmHHCPath != "") {
        const char *ext;
        char buffer[65536];
        unsigned int swath, offset;
        QString tmp="";
        QString HTML="";
        /* try to find the file */
        const char *filename = (const char *)chmHHCPath;
        if (chm_resolve_object(chmFile, filename, &m_ui) != CHM_RESOLVE_SUCCESS)
        {
            chmBuffer = "HELP";
            fsize = chmBuffer.length();
            return;
        }
	m_homestart = m_ui.start;
	m_homeend = m_homestart + m_ui.length;
        swath = 65536;
        offset = 0;
        fsize = m_ui.length;
        while (offset < m_ui.length)
        {
            if ((m_ui.length - offset) < 65536)
                swath = m_ui.length - offset;
            else
                swath = 65536;
            swath = (int)chm_retrieve_object(chmFile, &m_ui, (unsigned char *) buffer, offset, swath);
            tmp += buffer;
            offset += swath;
        }
        HTML +="<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN>\r\n";
        HTML +="<HTML>\r\n";
        HTML +="<BODY>\r\n";
        HTML +="<h2>Home Page</h2>\r\n";
        HTML +="<h3>\r\n";
        QStringList qsl = QStringList::split("\r\n",tmp);
        QString NameFind ="<param name=\"Name\" value=\"";
        QString LocalFind = "<param name=\"Local\" value=\"";
        QString ULOpenFind = "<UL>";
        QString LIOpenFind = "<LI>";
        QString ULCloseFind = "</UL>";
        QString Name = "";
        QString Local = "";
        for ( QStringList::Iterator it = qsl.begin(); it != qsl.end(); ++it ) {
            QString Line = (*it);
            int NamePos = Line.find( NameFind);
            int LocalPos = Line.find( LocalFind);
            if (NamePos > -1) {
                Name = Line.mid(NamePos+NameFind.length(),Line.length());
                Name = Name.left(Name.length() - 2);
            }
            if (LocalPos > -1) {
                Local = Line.mid(LocalPos+LocalFind.length(),Line.length());
                Local = Local.left(Local.length() - 2);
                if (Name != "") {
                    HTML += "<br>\r\n<a href=";
                    HTML += Local;
                    HTML += ">";
                    HTML += Name;
                    HTML += "</a>\r\n";
                    Name = "";
                }
            }
        }
        HTML +="</h3>";
        HTML +="</BODY>\r\n";
        HTML +="</HTML>\r\n";
        chmHomeBuffer = HTML;
        chmBuffer = HTML;
        fsize = chmBuffer.length();
	m_currentstart = m_ui.start;
	m_currentend = m_currentstart+chmBuffer.length();
    }
}

bool CHM::FillBuffer()
{
  bool bRetVal = false;
  char buffer[65536];
  int swath, offset;
  chmBuffer = "";
  swath = 65536;
  offset = 0;
  fsize = m_ui.length;
  while (offset < m_ui.length)
    {
      if ((m_ui.length - offset) < 65536)
	swath = m_ui.length - offset;
      else
	swath = 65536;
      swath = (int)chm_retrieve_object(chmFile, &m_ui, (unsigned char *) buffer, offset, swath);
      chmBuffer += buffer;
      offset += swath;
    }
  //There seems to be a ton of gobbledygook at the end that is reminiscent of the hhc file - remove it
  QString temp = chmBuffer.lower();
  int lpos = temp.find("</html");
  if (lpos > -1) {
    chmBuffer.truncate(lpos);
    chmBuffer = chmBuffer+"</HTML>";
  }
  fsize = chmBuffer.length();
  
  bufpos = 0;
  bRetVal = true;
  m_currentstart = m_ui.start;
  m_currentend = m_currentstart+chmBuffer.length();
  return bRetVal;
}

bool CHM::FillContent() {
    bool bRetVal = false;
    if (chmPath != "") {
        /* try to find the file */
        const char *filename = (const char *)chmPath;
        if (chm_resolve_object(chmFile, filename, &m_ui) != CHM_RESOLVE_SUCCESS)
        {
            fsize = chmBuffer.length();
            return bRetVal;
        }

        char buffer[65536];
        int swath, offset;
        chmBuffer = "";
        swath = 65536;
        offset = 0;
        fsize = m_ui.length;
        while (offset < m_ui.length)
        {
            if ((m_ui.length - offset) < 65536)
                swath = m_ui.length - offset;
            else
                swath = 65536;
            swath = (int)chm_retrieve_object(chmFile, &m_ui, (unsigned char *) buffer, offset, swath);
            chmBuffer += buffer;
            offset += swath;
        }
        //There seems to be a ton of gobbledygook at the end that is reminiscent of the hhc file - remove it
        QString temp = chmBuffer.lower();
        int lpos = temp.find("</html");
        if (lpos > -1) {
            chmBuffer.truncate(lpos);
            chmBuffer = chmBuffer+"</HTML>";
        }
        fsize = chmBuffer.length();

        bufpos = 0;
	m_currentstart = m_ui.start;
	m_currentend = m_currentstart+chmBuffer.length();
        bRetVal = true;
    }
    return bRetVal;
}

bool CHM::getFile(const QString& href) {
  qDebug("Got:%s", (const char*)href);
    bool bRetVal = false;
    QString temp = chmPath;
    chmPath = href;
    if (FillContent()) {
      qDebug("Got it");
        bRetVal = true;
    } else {
      qDebug("Missed");
        chmPath = temp;
        FillContent();
    }
    return bRetVal;
}

QImage *CHM::getPicture(const QString& href) {
        QImage *img = NULL;
        QString PicRef = "/"+href;
        struct chmUnitInfo ui;
        const char *ext;
        /* try to find the file */
        const char *filename = (const char *)PicRef;
        if (chm_resolve_object(chmFile, filename, &ui) != CHM_RESOLVE_SUCCESS)
        {
            qDebug ("Could not resolve image");
            return img;
        }
        if (ui.length > 0) {
            char *temp = new char[ui.length];
            char buffer[65536];
            int swath, offset;
            swath = 65536;
            offset = 0;

            while (offset < ui.length)
            {
                if ((ui.length - offset) < 65536)
                    swath = ui.length - offset;
                else
                    swath = 65536;
                swath = (int)chm_retrieve_object(chmFile, &ui, (unsigned char *) buffer, offset, swath);
                memcpy( (void *) (temp + offset), (void *)&buffer, swath );
                offset += swath;
            }
            QByteArray qba;
            qba.setRawData( (const char *)temp, ui.length);
            QPixmap qpm(qba);
            img = new QImage(qpm.convertToImage());
            qba.resetRawData((const char *)temp, ui.length);
            delete[] temp;
        }

        return img;
}

int CHM::OpenFile(const char *src) {
        if (chmFile != NULL) chm_close(chmFile);
        struct stat _stat;
        stat(src,&_stat);
        fsize = _stat.st_size;
        chmFile = chm_open(src);
        if (chmFile != NULL) {
            chm_enumerate(chmFile,
                        CHM_ENUMERATE_ALL,
                        _get_hhc,
                        (void *)this);
            if ( chmPath != "") {
                setHomePath(chmPath);
                FillHomeContent();
            }
        }
	m_homepos = locate();
	qDebug("Home:%u", m_homepos);
        return (chmFile==NULL);
}

int CHM::getch() {
    if ( (bufpos+1) >= chmBuffer.length() )
        return EOF;
#ifdef _WINDOWS
    QChar letter = chmBuffer.at(bufpos++);
#else
    QChar letter = chmBuffer[bufpos++];
#endif
    return (int)(char)letter;
}

void CHM::getch(tchar& ch, CStyle& sty)
{
        int ich = getch();
        ch = (ich == EOF) ? UEOF : ich;
}

void CHM::start2endSection()
{
  m_currentstart = m_ui.start;
  m_currentend = m_currentstart+chmBuffer.length();
}

unsigned int CHM::locate() {
    return m_currentstart+bufpos;
}

void CHM::locate(unsigned int n) {
  if (n == 0) n = m_homepos;
  if (n >= m_homestart && n < m_homeend)
    {
      FillHomeContent();
      bufpos = n - m_homestart;
      start2endSection();
      qDebug("Home:%u:<%u, %u, %u>", m_ui.start, m_currentstart, n, m_currentend);
    }
  else if (n >= m_currentstart && n < m_currentend)
    {
      bufpos = n - m_currentstart;
    }
  else if (CHM_RESOLVE_FAILURE == chm_resolve_location(chmFile, n/4, &m_ui))
    {
      qDebug("Resolve failure");
      FillHomeContent();
      bufpos = 0;
      start2endSection();
      qDebug("Home:%u:<%u, %u, %u>", m_ui.start, m_currentstart, n, m_currentend);
    }
  else
    {
      qDebug("Resolve success");
      FillBuffer();
      start2endSection();
      bufpos=n-m_currentstart;
      qDebug("Not Home:%u:<%u, %u, %u>", m_ui.start, m_currentstart, n, m_currentend);
    }
}

void CHM::sizes(unsigned long& _file, unsigned long& _text) {
    _text = _file = fsize;
}
