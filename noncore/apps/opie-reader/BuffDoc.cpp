
#define NEWLINEBREAK

#include "BuffDoc.h"
//#include <FL/fl_draw.h>
#include "plucker.h"
#ifdef USENEF
#include "nef.h"
#include "arrierego.h"
#endif

linkType BuffDoc::hyperlink(unsigned int n, QString& wrd)
{
    linkType bRet = eNone;
    if (exp != NULL)
    {
	bRet = exp->hyperlink(n, wrd);
	if (bRet == eLink)
	{
	    lastword.empty();
	    lastsizes[0] = laststartline = n;
#ifdef NEWLINEBREAK
	    lastispara = true;
#else
	    lastispara = false;
#endif
	    lastsizes[0] = laststartline = exp->locate();
	}
    }
    return bRet;
}

void BuffDoc::locate(unsigned int n)
{
  //  //qDebug("BuffDoc:locating:%u",n);
    lastword.empty();
  lastsizes[0] = laststartline = n;
#ifdef NEWLINEBREAK
    lastispara = true;
#else
    lastispara = false;
#endif
  //    tchar linebuf[1024];
  if (exp != NULL) exp->locate(n);
  //  //qDebug("BuffDoc:Located");
}

#ifdef NEWLINEBREAK
bool BuffDoc::getline(CDrawBuffer* buff, int wth, unsigned char _border)
{
    bool moreleft = true;
    bool margindone = false;
    int w = wth-2*_border;
    tchar ch = 32;
    CStyle cs;
    buff->empty();
    if (exp == NULL)
    {
	buff->empty();
	buff->setEof();
	return false;
    }
    int len = 0;
    if (lastword.length() > 0)
    {
	*buff = lastword;
	cs = lastword.laststyle();
	w -= buff->leftMargin() + buff->rightMargin();
	margindone = true;
	len = lastword.length();
    }
    else buff->empty();
    lastword.empty();
    unsigned int slen = buff->width(len);
    if (lastispara) buff->setstartpara();
    while (1)
    {
	lastsizes[len] = exp->locate();
	getch(ch, cs);
	if (ch == 10 && len == 0 && !lastispara)
	{
	    lastsizes[len] = exp->locate();
	    getch(ch, cs);
	}
	if (ch == UEOF)
	{
	    if (len == 0)
	    {
		buff->setEof();
		moreleft = false;
	    }
	    laststartline = exp->locate();
	    break;
	}
	if (ch == 10)
	{
	    buff->setendpara();
	    lastispara = true;
	    laststartline = exp->locate();
	    break;
	}
	lastispara = false;
	buff->addch(ch, cs);
	len++;
	if (!margindone)
	{
	    w -= buff->leftMargin() + buff->rightMargin();
	    margindone = true;
	}
  	if ((slen = buff->width(len)) > w)
	{
	    if (ch == ' ' || len == 1)
	    {
		if (ch == ' ') buff->truncate(len-1);
		laststartline = exp->locate();
		break;
	    }
	    else // should do a backward search for spaces, first.
	    {
		for (int i = len-2; i > 0; i--)
		{
		    if ((*buff)[i] == ' ')
		    {
			(*buff)[len] = 0;
			lastword.setright(*buff, i+1);
			buff->truncate(i);
			(*buff)[i] = '\0';
			laststartline = lastsizes[i+1];
			buff->resize();
			for (int j = 0; j < lastword.length(); j++)
			{
			    lastsizes[j] = lastsizes[j+i+1];
			}
			return true;
		    }
		    if ((*buff)[i] == '-' && !(((*buff)[i-1] == '-') || ((*buff)[i+1] == '-')))
		    {
			(*buff)[len] = 0;
			lastword.setright(*buff, i+1);
			buff->truncate(i+1);
			(*buff)[i+1] = '\0';
			laststartline = lastsizes[i+1];
			buff->resize();
			for (int j = 0; j < lastword.length(); j++)
			{
			    lastsizes[j] = lastsizes[j+i+1];
			}
			return true;
		    }
		}
		laststartline = lastsizes[len-1];
		(*buff)[len] = 0;
		lastword.setright(*buff, len - 1);
		buff->truncate(len-1);
		buff->addch('-', cs);
		for (int j = 0; j < lastword.length(); j++)
		{
		    lastsizes[j] = lastsizes[j+len];
		}
		break;
	    }
	}
    }
    (*buff)[len] = '\0';
    buff->resize();
    return moreleft;
}
#else
bool BuffDoc::getline(CDrawBuffer* buff, int wth, unsigned char _border)
{
    bool margindone = false;
    int w = wth-2*_border;
    tchar ch = 32;
    CStyle cs;
    buff->empty();
    if (exp == NULL)
    {
//	(*buff)[0] = '\0';
	buff->empty();
	return false;
    }
  int len = 0, lastcheck = 0;
  if (lastword.length() > 0)
  {
      *buff = lastword;
      cs = lastword.laststyle();
      w -= buff->leftMargin() + buff->rightMargin();
      margindone = true;
  }
  else buff->empty();
//  //qDebug("Buff:%s Lastword:%s", (const char*)toQString(buff->data()), (const char*)toQString(lastword.data()));
  lastcheck = len = buff->length();
  unsigned int slen = buff->width(len);
  if (slen > w)
    {
      for ( ; len > 1; len--)
	{
	  if (buff->width(len) < w) break;
	}
//      lastword = buff->data() + len - 1;
      laststartline = lastsizes[len-1];
      for (int i = 0; i < buff->length(); i++) lastsizes[i] = lastsizes[i+len-1];
//      (*buff)[len-1] = '-';
      if (len > 2)
      {
	  lastword.setright(*buff, len - 1);
	  buff->truncate(len-1);
	  buff->addch('-', cs);
	  (*buff)[len] = '\0';
      }

      else
      {
	  lastword.empty();
	  (*buff)[len] = '\0';
      }
      buff->resize();
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
      buff->resize();
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
      if (!margindone)
      {
	  w -= buff->leftMargin() + buff->rightMargin();
	  margindone = true;
      }
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
#ifdef _WINDOWS
  lastword.setright(*buff, (lastcheck > 0) ? lastcheck : 1);
  {
	  int i;
  for (i = 0; i < lastword.length(); i++) lastsizes[i] = allsizes[i+lastcheck];
  }
#else
  lastword.setright(*buff, (lastcheck > 0) ? lastcheck : 1);
  for (int i = 0; i < lastword.length(); i++) lastsizes[i] = allsizes[i+lastcheck];
#endif
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
  buff->resize();
  if (ch == UEOF && buff->length() == 0)
  {
      buff->setEof();
      return false;
  }
  return true;
}
#endif

bool BuffDoc::getline(CDrawBuffer* buff, int wth, int cw, unsigned char _border)
{
    int w = wth-2*_border;
    buff->empty();
    if (exp == NULL)
    {
	return false;
    }
    tchar ch;
    CStyle cs;
    int i = 1;
    while (i*cw < w-buff->offset(w,0))
    {
	getch(ch, cs);
	if (ch == '\12' || ch == UEOF) break;
	buff->addch(ch,cs);
	i++;
    }
    buff->truncate(i);
    laststartline = exp->locate();
    buff->resize();
    return (ch != UEOF);
}

int BuffDoc::openfile(QWidget* _parent, const char *src)
{
    //    //qDebug("BuffDoc:Openfile:%s", src);
    //    //qDebug("Trying aportis %x",exp);
    if (exp != NULL) delete exp;
    lastword.empty();
    lastsizes[0] = laststartline = 0;
#ifdef NEWLINEBREAK
    lastispara = true;
#else
    lastispara = false;
#endif
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
#ifdef USENEF
    if (ret != 0)
    {

	delete exp;
	exp = new CArriere;
	ret = exp->openfile(src);
    }
    if (ret != 0)
    {

	delete exp;
	exp = new CNEF;
	ret = exp->openfile(src);
    }
#endif
    if (ret != 0)
    {

	delete exp;
	exp = new CPlucker;
	ret = exp->openfile(src);
    }
    if (ret != 0)
    {
	delete exp;
	//qDebug("Trying ppms");
	exp = new ppm_expander;
	ret = exp->openfile(src);
    }
    if (ret != 0)
    {
	delete exp;
	exp = new Text;
//		//qDebug("Trying text");
	ret = exp->openfile(src);
    }

    if (ret != 0)
    {
	delete exp;
	QMessageBox::information(_parent, PROGNAME, "Unknown file compression type","Try another file");
	return ret;
    }
    //        //qDebug("Doing final open:%x:%x",exp,filt);

    lastword.empty();
    lastsizes[0] = laststartline = 0;
#ifdef NEWLINEBREAK
    lastispara = true;
#else
    lastispara = false;
#endif
    exp->locate(0);
    filt->setsource(exp);
    //        //qDebug("BuffDoc:file opened");
    return 0;
}
