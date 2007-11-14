#include "names.h"

#define NEWLINEBREAK
#define INCREMENTALWIDTH

#include "BuffDoc.h"
#include "config.h"
#include "CDrawBuffer.h"
#include "ZText.h"
#include "ebookcodec.h"

#ifdef __STATIC
#include "Aportis.h"
#include "CHM.h"
#include "ppm_expander.h"
#include "ztxt.h"
#include "plucker.h"
#ifdef USENEF
#include "nef.h"
#include "arrierego.h"
#endif
#include "iSilo.h"
#include "Reb.h"
#endif


linkType BuffDoc::hyperlink(unsigned int n, unsigned int noff, QString& wrd, QString& nm)
{
    linkType bRet = eNone;
    if (exp != NULL)
    {
	bRet = filt->hyperlink(n, noff, wrd, nm);
	if ((bRet & eLink) != 0)
	{
	    lastword.empty();
	    lastsizes[0] = laststartline = n;
#ifdef NEWLINEBREAK
	    lastispara = false;
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
    lastispara = false;
#else
    lastispara = false;
#endif
  //    tchar linebuf[1024];
  if (exp != NULL) filt->locate(n);
  //  //qDebug("BuffDoc:Located");
}

void BuffDoc::resetPos()
{
  //  //qDebug("BuffDoc:locating:%u",n);
    lastword.empty();
  lastsizes[0] = laststartline = exp->locate();
#ifdef NEWLINEBREAK
    lastispara = false;
#else
    lastispara = false;
#endif
}

static bool isletter(unsigned short c)
{
  if ('a' <= c && c <= 'z') return true;
  if ('A' <= c && c <= 'Z') return true;
  // Cyrillic letters
  if (0x400 <= c && c <= 0x52F) return true;
  return false;
}

static bool isvowel(unsigned short c) // Only want lower case vowels
{
  switch (c)
    {
    case 'a':
    case 'e':
    case 'i':
    case 'o':
    case 'u':
      // Cyrillic vowels
    case 0x430:
    case 0x435:
    case 0x438:
    case 0x43E:
    case 0x443:
    case 0x44B:
    case 0x44D:
    case 0x44E:
    case 0x44F:
    case 0x451:
      
    case 0x450:
    case 0x454:
    case 0x456:
    case 0x457:
    case 0x45D:
      
    case 0x463:
      
    case 0x4AF:
    case 0x4B1:
    case 0x4D1:
    case 0x4D3:
      
    case 0x4D5:
    case 0x4D7:
    case 0x4E3:
    case 0x4E5:
    case 0x4E7:
    case 0x4E9:
    case 0x4EB:
    case 0x4ED:
    case 0x4EF:
    case 0x4F1:
    case 0x4F3:
    case 0x4F9:
      return true;
    default:
      return ((0xe0 <= c) && (c <= 0xfc) && (c != 0xf1) && (c != 0xf7) && (c != 0xe7));
    }
}

inline bool isconsonant(unsigned short c)
{
  switch( c )
    {
      // Some cyrillic letters are neither vowels, nor consonants, or otherwise
      // the letters no word can start from.
    case 0x439:
    case 0x44A:
    case 0x44C:
      
    case 0x45E:
      
      return false;
      
    default:
      break;
    }
  return (isletter(c) && !isvowel(c));
}

#ifdef NEWLINEBREAK
bool BuffDoc::getline(CDrawBuffer* buff, int wth, unsigned short _lborder, unsigned short _rborder, bool hyphenate, int availht)
{
    bool moreleft = true;
    bool margindone = false;
    int isCentred = -1;
    int w = wth-(_lborder+_rborder);
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
	if (isCentred < 0)
	  {
	    isCentred = (lastword.firststyle().getJustify() == m_AlignCentre) ? 1 : 0;
	  }
	*buff = lastword;
	cs = lastword.laststyle();
	w -= buff->leftMargin() + buff->rightMargin();
	margindone = true;
	len = lastword.length();
    }
    else buff->empty();
    lastword.empty();
    unsigned int slen = buff->width(availht, len);
    if (lastispara) buff->setstartpara();
    int nospaces = 0;
    while (1)
    {
	getch(ch, cs, lastsizes[len]);
	if (isCentred < 0)
	  {
	    isCentred = (cs.getJustify() == m_AlignCentre) ? 1 : 0;
	  }
	if (ch == 10 && len == 0 && !lastispara)
	{
	    getch(ch, cs, lastsizes[len]);
	    buff->setstartpara();
	}
	if (ch == UEOF)
	{
	    buff->setendpara(cs);
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
	    buff->setendpara(cs);
	    lastispara = true;
	    laststartline = exp->locate();
	    break;
	}
	if (ch == 6)
	{
	    buff->setendpara(cs);
	    buff->setBop();
	    lastispara = true;
	    laststartline = exp->locate();
	    break;
	}
	lastispara = false;
	buff->addch(ch, cs);
	if (ch == ' ')
	  {
	    nospaces++;
	  }
	len++;
	if (!margindone)
	{
	    w -= buff->leftMargin() + buff->rightMargin();
	    margindone = true;
	}
#ifdef INCREMENTALWIDTH
  	if ((slen += buff->charwidth(len-1, cs)) > w)
#else
  	if ((slen = buff->width(availht, len)) > w)
#endif
	{
	    if (ch == ' ' || len == 1)
	    {
		if (ch == ' ') buff->truncate(len-1);
		laststartline = exp->locate();
		break;
	    }
	    else // should do a backward search for spaces, first.
	    {
	      int lastk = len-4;
		for (int i = len-2; i > 0; i--)
		{
		    if (
			(((*buff)[i] == 0x2014) && isletter((*buff)[i+1]))
			||
			((*buff)[i] == '-')
			//			((*buff)[i] == '-' && !(((*buff)[i-1] == '-') || ((*buff)[i+1] == '-')))
)
		    {
			(*buff)[len] = 0;
			lastword.setright(*buff, i+1);
			buff->truncate(i+1);
			(*buff)[i+1] = '\0';
			laststartline = lastsizes[i+1];
			//			buff->resize();
			for (int j = 0; j < lastword.length(); j++)
			{
			    lastsizes[j] = lastsizes[j+i+1];
			}
			return true;
		    }

		    //if (hyphenate && (m_hyphenthreshold+1)*i < m_hyphenthreshold*len)
		    // We end up with i+1 characters instead of len-1
		    // Thus we are removing len - 1 - (i + 1) = len-i-2
		    // The space characters will be stretched to cover
		    // nospaces to nospaces + len - i - 2
		    // The stretch factor is hence
		    // (nospaces+len-i-2)/nospaces
		    if (hyphenate && !isCentred && ( 100*(nospaces+len-i-2) > (100+m_hyphenthreshold)*nospaces ))
		      {
			/*
			if (m_customhyphen)
			  {
			    for (int k = lastk; k >= i && k >= 2; k--)
			      {
				if (
				    isletter((*buff)[k+3])
				    &&
				    isletter((*buff)[k+2])
				    &&
				    isvowel((*buff)[k+1])
				    &&
				    isconsonant((*buff)[k])
				    &&
				    isletter((*buff)[k-1])
				    &&
				    isletter((*buff)[k-2])
				    )
				  {
				    (*buff)[len] = 0;
				    lastword.setright(*buff, k+1);
				    buff->truncate(k+2);
				    (*buff)[k+1] = '-';
				    (*buff)[k+2] = '\0';
				    laststartline = lastsizes[k+1];
				    buff->resize();
				    for (int j = 0; j < lastword.length(); j++)
				      {
					lastsizes[j] = lastsizes[j+k+1];
				      }
				    return true;
				  }
			      }
			  }
			else
			*/
			  {
			    for (int k = lastk; k >= i && k >= 1; k--)
			      {
				if (
				    /*
				      (
				      k < len-3
				      &&
				      k >= 1
				      &&
				      isletter((*buff)[k+3])
				      &&
				      isletter((*buff)[k+2])
				      &&
				      isconsonant((*buff)[k+1])
				      &&
				      ((*buff)[k+1] != 'y')
				      &&
				      ((*buff)[k+1] != 'h')
				      &&
				      isconsonant((*buff)[k])
				      &&
				      ((*buff)[k] != 'h')
				      &&
				      isletter((*buff)[k-1])
				      )
				      ||
				    */
				    (
				     isletter((*buff)[k+3])
				     &&
				     isletter((*buff)[k+2])
				     &&
				     isconsonant((*buff)[k+1])
				     &&
				     ((*buff)[k+1] != 'y')
				     &&
				     isletter((*buff)[k])
				     &&
				     ((*buff)[k] != 'h')
				     &&
				     isletter((*buff)[k-1])
				     && // Do not hyphenate when the first part ends with a vowel,
				     // and the second starts with the two consonants.
				     // Examples: "co-nsona-nts" -> "con-sonants",
				     // "hy-phenation" -> "hyp-henation" etc.
				     !( isvowel( (*buff)[k] ) && isconsonant( (*buff)[k+2] ) )
				     )
				    )
				  {
				    (*buff)[len] = 0;
				    lastword.setright(*buff, k+1);
				    buff->truncate(k+2);
				    (*buff)[k+1] = '-';
				    (*buff)[k+2] = '\0';
				    laststartline = lastsizes[k+1];
				    //				    buff->resize();
				    for (int j = 0; j < lastword.length(); j++)
				      {
					lastsizes[j] = lastsizes[j+k+1];
				      }
				    return true;
				  }
			      }
			  }
			lastk = i;
		      }
		    if ((*buff)[i] == ' ')
		    {
			(*buff)[len] = 0;
			lastword.setright(*buff, i+1);
			buff->truncate(i);
			(*buff)[i] = '\0';
			laststartline = lastsizes[i+1];
			//			buff->resize();
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
    //    buff->resize();
    return moreleft;
}
#else
bool BuffDoc::getline(CDrawBuffer* buff, int wth, unsigned short _lborder, unsigned short _rborder)
{
    bool margindone = false;
    int w = wth-(_lborder+_rborder);
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
  unsigned int slen = buff->width(availht, len);
  if (slen > w)
    {
      for ( ; len > 1; len--)
	{
	  if (buff->width(availht, len) < w) break;
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
      //      buff->resize();
      return true;
    }
  if (lastispara)
    {
      lastispara = false;
//      lastword[0] = '\0';
      lastword.empty();
      len = buff->length();
      while (buff->width(availht, len) > w) len--;
//      (*buff)[len] = '\0';
      buff->truncate(len);
      laststartline = exp->locate();
      //      buff->resize();
      return true;
    }
  lastispara = false;
  for (int i = 0; i < len; i++) allsizes[i] = lastsizes[i];
  while (slen < w)
  {
      lastcheck = len;
      getch(ch, cs, allsizes[len]);
      while (ch != ' ' && ch != '\012' && ch != UEOF && len < 128)
      {
	  len++;
	  buff->addch(ch,cs);
	  getch(ch, cs, allsizes[len]);
      }
      (*buff)[len] = 0;
      slen = buff->width(availht, len);
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
//  buff->resize();
  if (ch == UEOF && buff->length() == 0)
  {
      buff->setEof();
      return false;
  }
  return true;
}
#endif

bool BuffDoc::getline(CDrawBuffer* buff, int wth, int cw, unsigned short _lborder, unsigned short _rborder, int availht)
{
    int w = wth-(_lborder+_rborder);
    buff->empty();
    if (exp == NULL)
    {
	return false;
    }
    tchar ch;
    CStyle cs;
    int i = 1;
    int offset = buff->offset(w,0,0,availht);
    while (i*cw < w-offset)
    {
      unsigned long dummy;
	getch(ch, cs, dummy);
	if (ch == '\12' || ch == UEOF) break;
	buff->addch(ch,cs);
	i++;
    }
    buff->truncate(i);
    laststartline = exp->locate();
    //    buff->resize();
    return (ch != UEOF);
}

int BuffDoc::openfile(QWidget* _parent, const char *src)
{
  if (exp != NULL) delete exp;
  exp = NULL;

#ifdef __STATIC
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
    /*
      }
    if (ret != 0)
    {
	delete exp;
	exp = new ebookcodec("Aportis");
	ret = exp->openfile(src);
	if (ret == 0) qDebug("Aportis format");
    }
    */
#ifdef USENEF
    if (ret != 0)
    {

	delete exp;
	exp = new CArriere;
	ret = exp->openfile(src);
	if (ret == 0) qDebug("Arriere format");
    }
    if (ret != 0)
    {

	delete exp;
	exp = new CNEF;
	ret = exp->openfile(src);
	if (ret == 0) qDebug("NEF format");
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
	exp = new CHM;
	ret = exp->openfile(src);
    }
    if (ret != 0)
    {

	delete exp;
	exp = new iSilo;
	ret = exp->openfile(src);
    }
    if (ret != 0)
    {
	delete exp;
	//qDebug("Trying ppms");
	exp = new CReb;
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
	if (ret == 0) qDebug("Text format");
    }

    if (ret != 0)
    {
	delete exp;
	QMessageBox::information(_parent, PROGNAME, "Unknown file compression type","Try another file");
	return ret;
    }
    //        //qDebug("Doing final open:%x:%x",exp,filt);
#else
#ifdef USEQPE
#ifdef OPIE
  QString codecpath(getenv("OPIEDIR"));
#else
  QString codecpath(getenv("QTDIR"));
#endif
  codecpath += "/plugins/reader/codecs";
#else
  QString codecpath(getenv("READERDIR"));
  codecpath += "/codecs";
#endif
  QDir d(codecpath, "*.so");

  if (d.exists())
    {

      const QFileInfoList *list = d.entryInfoList();
      QFileInfoListIterator it( *list );      // create list iterator
      QFileInfo *fi;                          // pointer for traversing
  
      int ret = -1;
      while ( ret != 0 && (fi=it.current()) )
	{           // for each file...
	  if (exp != NULL) delete exp;
	  qDebug("Trying %s", (const char*)fi->fileName());
	  exp = new ebookcodec(fi->fileName());
	  ret = exp->openfile(src);
	  ++it;
	}
      qDebug("Buffdoc:Finished opening");
      if (ret != 0)
	{
	  if (exp != NULL) delete exp;
	  exp = new Text;
	  ret = exp->openfile(src);
	}
    }
  else
    {
      if (exp != NULL) delete exp;
      exp = new Text;
      exp->openfile(src);
    }
#endif
  lastword.empty();
  lastsizes[0] = laststartline = 0;
#ifdef NEWLINEBREAK
  lastispara = false;
#else
  lastispara = false;
#endif
  exp->locate(exp->getHome());
  filt->setsource(exp);
  qDebug("BuffDoc:file opened");
  return 0;
}

QString BuffDoc::about()
{
  return QString("Buffered Decompressor (c) Tim Wentford\nHyphenation algorithm (c) Tim Wentford\n  (Cyrillic support by Konstantin Isakov\n")+filt->about();
}

int BuffDoc::getsentence(CBuffer& buff)
{
  tchar ch;
  int i = 0;
  bool intext = false;
  while ((ch = getch()) != 10 && ch != UEOF)
    {
      buff[i++] = ch;
      if (ch == '"' || ch == '\'' || ch == 0x2018 || ch == 0x2019 ||
	  ch == 0x201a || ch == 0x201b || ch == 0x201c || ch == 0x201d)
	{
	  intext = !intext;
	}
      if (!intext && (ch == '.' || ch == '!' || ch == '?')) break;
   }

  buff[i] = '\0';
  if (i == 0 && ch == UEOF) i = -1;
  laststartline = exp->locate();
  return i;
}
