#include "BuffDoc.h"
//#include <FL/fl_draw.h>
#include "config.h"
#include "CDrawBuffer.h"
#include "plucker.h"


bool BuffDoc::hyperlink(unsigned int n)
{
    bool bRet = false;
    lastword.empty();
    lastsizes[0] = laststartline = n;
    lastispara = false;
    if (exp != NULL)
    {
  bRet = exp->hyperlink(n);
  lastsizes[0] = laststartline = exp->locate();
    }
    return bRet;
}

void BuffDoc::locate(unsigned int n)
{
  //  qDebug("BuffDoc:locating:%u",n);
    lastword.empty();
  lastsizes[0] = laststartline = n;
  lastispara = false;
  //    tchar linebuf[1024];
  if (exp != NULL) exp->locate(n);
  //  qDebug("BuffDoc:Located");
}

bool BuffDoc::getline(CDrawBuffer* buff, int w)
{
    tchar ch = 32;
    CStyle cs;
    buff->empty();
    if (exp == NULL)
    {
//  (*buff)[0] = '\0';
  buff->empty();
  return false;
    }
  int len = 0, lastcheck = 0;
  if (lastword.length() > 0)
  {
      *buff = lastword;
      cs = lastword.laststyle();
  }
  else buff->empty();
//  qDebug("Buff:%s Lastword:%s", (const char*)toQString(buff->data()), (const char*)toQString(lastword.data()));
  lastcheck = len = buff->length();
  unsigned int slen = buff->width(len);
  if (slen > w)
    {
      for ( ; len > 0; len--)
  {
    if (buff->width(len) < w) break;
  }
//      lastword = buff->data() + len - 1;
      laststartline = lastsizes[len-1];
      lastword.setright(*buff, len - 1);
      for (int i = 0; i < buff->length(); i++) lastsizes[i] = lastsizes[i+len-1];
//      (*buff)[len-1] = '-';
      buff->truncate(len-1);
      buff->addch('-', cs);
      (*buff)[len] = '\0';
      return true;
    }
  if (lastispara)
    {
      lastispara = false;
//      lastword[0] = '\0';
      lastword.empty();
      len = buff->length();
      while (buff->width(len) > w) len--;
//      (*buff)[len] = '\0';
      buff->truncate(len);
      laststartline = exp->locate();
      return true;
    }
  lastispara = false;
  for (int i = 0; i < len; i++) allsizes[i] = lastsizes[i];
  while (slen < w)
  {
      lastcheck = len;
      allsizes[len] = exp->locate();
      getch(ch, cs);
      while (ch != ' ' && ch != '\012' && ch != UEOF && len < 128)
      {
    len++;
    buff->addch(ch,cs);
    allsizes[len] = exp->locate();
    getch(ch, cs);
      }
      (*buff)[len] = 0;
      slen = buff->width(len);
      len++;
      buff->addch(' ', cs);
      allsizes[len] = exp->locate();
      if (slen < w && ch != ' ')
      {
    lastcheck = len;
    break;
      }
      lastispara = (ch == '\012');
  }
  (*buff)[len] = '\0';
//  lastword = buff->data()+lastcheck;
  lastword.setright(*buff, lastcheck);
  for (int i = 0; i < lastword.length(); i++) lastsizes[i] = allsizes[i+lastcheck];
  if (lastcheck > 0)
  {
      laststartline = allsizes[lastcheck];
//      (*buff)[lastcheck-1] = '\0';
      buff->truncate(lastcheck-1);
  }
  else
  {
      laststartline = (lastcheck == len) ? exp->locate() : allsizes[lastcheck+1];
//      (*buff)[lastcheck] = '\0';
      buff->truncate(lastcheck);
  }
//  buff->frig();
  return (ch != UEOF);
}

bool BuffDoc::getline(CDrawBuffer* buff, int w, int cw)
{
    buff->empty();
    if (exp == NULL)
    {
  return false;
    }
    tchar ch;
    CStyle cs;
    int i = 0;
    while (i*cw < w)
    {
  getch(ch, cs);
  if (ch == '\12' || ch == UEOF) break;
  buff->addch(ch,cs);
  i++;
    }
    buff->truncate(i);
    laststartline = exp->locate();
    return (ch != UEOF);
}

int BuffDoc::openfile(QWidget* _parent, const char *src)
{
    //    qDebug("BuffDoc:Openfile:%s", src);
    //    qDebug("Trying aportis %x",exp);
    if (exp != NULL) delete exp;
    lastword.empty();
    lastsizes[0] = laststartline = 0;
    lastispara = false;
    /*
      exp = new Text;
      int ret = exp->openfile(src);
    */

    exp = new Aportis;
    int ret = exp->openfile(src);
    if (ret == -1)
    {
  delete exp;
  exp = NULL;
  return ret;
    }
    if (ret == -2)
    {

  delete exp;
  exp = new ztxt;
  ret = exp->openfile(src);
    }
    if (ret != 0)
    {

  delete exp;
  exp = new CPlucker;
  ret = exp->openfile(src);
    }
#ifndef SMALL
    if (ret != 0)
    {
  delete exp;
  qDebug("Trying ppms");
  exp = new ppm_expander;
  ret = exp->openfile(src);
    }
      
    if (ret != 0)
    {
  delete exp;
  exp = new Text;
//    qDebug("Trying text");
  ret = exp->openfile(src);
    }
#else
    if (ret != 0)
    {
  delete exp;
  exp = new Text;
  ret = exp->openfile(src);
    }
#endif
    if (ret != 0)
    {
  delete exp;
  QMessageBox::information(_parent, "OpieReader", "Unknown file compression type","Try another file");
  return ret;
    }
    //        qDebug("Doing final open:%x:%x",exp,filt);

    lastword.empty();
    lastsizes[0] = laststartline = 0;
    lastispara = false;
    exp->locate(0);
    filt->setsource(exp);
    //        qDebug("BuffDoc:file opened");
    return 0;
}
