#include "CExpander.h"
#ifdef USEQPE
#include <qpe/global.h>
#endif

size_t CExpander::getHome() { return m_homepos; }

CExpander::CExpander() : m_homepos(0), fname(NULL), m_scrWidth(240), m_currentstart(1), m_currentend(0) {};

CExpander::~CExpander() { if (fname != NULL) delete [] fname; };

int CExpander::openfile(const char *src)
{
  bSuspended = false;
  fname = strdup(src);
  return OpenFile(src);
}

void CExpander::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  pos = locate();
  int ich = getch();
  ch = (ich == EOF) ? UEOF : ich;
  //	    sty.unset();
}

linkType CExpander::hyperlink(unsigned int n, unsigned int, QString& wrd, QString& nm)
{
  locate(n);
  return eLink;
}

unsigned long CExpander::startSection()
{
  unsigned long current = locate();
  if (m_currentstart > current || current > m_currentend)
    {
      start2endSection();
    }
  return m_currentstart;
}

unsigned long CExpander::endSection()
{
  unsigned long current = locate();
  if (m_currentstart > current || current > m_currentend)
    {
      start2endSection();
    }
  return m_currentend;
}

void CExpander::start2endSection()
{
  m_currentstart = 0;
  unsigned long file;
  sizes(file, m_currentend);
}

#ifdef USEQPE
void CExpander::suspend(FILE*& fin)
{
  bSuspended = true;
  suspos = ftell(fin);
  fclose(fin);
  fin = NULL;
  sustime = time(NULL);
}

void CExpander::unsuspend(FILE*& fin)
{
  if (bSuspended)
    {
      bSuspended = false;
      int delay = time(NULL) - sustime;
      if (delay < 10)
	{
	  Global::statusMessage("Stalling");
	  sleep(10-delay);
	}
      fin = fopen(fname, "rb");
      for (int i = 0; fin == NULL && i < 5; i++)
	{
	  Global::statusMessage("Stalling");
	  sleep(5);
	  fin = fopen(fname, "rb");
	}
      if (fin == NULL)
	{
	  QMessageBox::warning(NULL, PROGNAME, "Couldn't reopen file");
	  exit(0);
	}
      suspos = fseek(fin, suspos, SEEK_SET);
    }
}
#endif

void CExpander::setSaveData(unsigned char*& data, unsigned short& len, unsigned char* src, unsigned short srclen)
{
  len = srclen;
  data = new unsigned char[len];
  memcpy(data, src, len);
}

void CExpander::putSaveData(unsigned char*& src, unsigned short& srclen)
{
  if (srclen != 0)
    {
      qDebug("Don't know what to do with non-zero save data");
    }
}
