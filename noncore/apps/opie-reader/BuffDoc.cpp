#include "BuffDoc.h"
//#include <FL/fl_draw.h>
#include "config.h"

void BuffDoc::locate(unsigned int n)
{
  //  qDebug("BuffDoc:locating:%u",n);

  lastword[0] = '\0';
  lastsizes[0] = laststartline = n;
  lastispara = false;
  //    tchar linebuf[1024];
  if (exp != NULL) exp->locate(n);
  //  qDebug("BuffDoc:Located");
}

bool BuffDoc::getline(CBuffer* buff, int w)
{
    if (exp == NULL)
    {
	(*buff)[0] = '\0';
	return false;
    }
  int len = 0, ch, lastcheck = 0;
  *buff = lastword.data();
  lastcheck = len = buff->length();
  //  unsigned int slen = fl_widthi(buff->data(),len);
#ifdef _UNICODE
  unsigned int slen = m_fm->width(toQString(buff->data()),len);
#else
  unsigned int slen = m_fm->width(buff->data(),len);
#endif
  if (slen > w)
    {
      for ( ; len > 0; len--)
	{
	  //	  if (fl_widthi(buff->data(),len) < w) break;
#ifdef _UNICODE
	  if (m_fm->width(toQString(buff->data()),len) < w) break;
#else
	  if (m_fm->width(buff->data(),len) < w) break;
#endif
	}
      lastword = buff->data() + len - 1;
      for (int i = 0; i < buff->length(); i++) lastsizes[i] = lastsizes[i+len-1];
      (*buff)[len-1] = '-';
      (*buff)[len] = '\0';
      laststartline = lastsizes[len-1];
      return true;
    }
  if (lastispara)
    {
      lastispara = false;
      lastword[0] = '\0';
      len = buff->length();
      //      while (fl_widthi(buff->data(),len) > w) len--;
#ifdef _UNICODE
      while (m_fm->width(toQString(buff->data()),len) > w) len--;
#else
      while (m_fm->width(buff->data(),len) > w) len--;
#endif
      (*buff)[len] = '\0';
      laststartline = exp->locate();
      return true;
    }
  lastispara = false;
  for (int i = 0; i < len; i++) allsizes[i] = lastsizes[i];
  while (slen < w)
  {
      lastcheck = len;
      allsizes[len] = exp->locate();
      while ((ch = getch()) != ' ' && ch != '\012' && ch != UEOF && len < 128)
      {
	  (*buff)[len++] = ch;
	  allsizes[len] = exp->locate();
      }
      (*buff)[len] = 0;
#ifdef _UNICODE
      slen = m_fm->width(toQString(buff->data()),len);
#else
      slen = m_fm->width(buff->data(),len);
#endif
      (*buff)[len++] = ' ';
      allsizes[len] = exp->locate();
      if (slen < w && ch != ' ')
      {
	  lastcheck = len;
	  break;
      }
      lastispara = (ch == '\012');
  }
  (*buff)[len] = '\0';
  lastword = buff->data()+lastcheck;
  for (int i = 0; i < lastword.length(); i++) lastsizes[i] = allsizes[i+lastcheck];
  if (lastcheck > 0)
  {
      laststartline = allsizes[lastcheck];
      (*buff)[lastcheck-1] = '\0';
  }
  else
  {
      laststartline = (lastcheck == len) ? exp->locate() : allsizes[lastcheck+1];
      (*buff)[lastcheck] = '\0';
  }
//  laststartline = sizes[lastcheck+1];
//  (*buff)[(lastcheck > 0) ? lastcheck-1:lastcheck] = '\0';
  return (ch != UEOF);
}

bool BuffDoc::getline(CBuffer* buff, int w, int cw)
{
    if (exp == NULL)
    {
	(*buff)[0] = '\0';
	return false;
    }
    tchar ch;
    int i = 0;
    while ((i < w/cw) && ((ch = getch()) != '\012') && (ch != UEOF))
    {
	(*buff)[i++] = ch;
    }
    (*buff)[i] = '\0';
    laststartline = exp->locate();
    return (ch != UEOF);
}
