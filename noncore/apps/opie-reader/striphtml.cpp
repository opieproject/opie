#include <qmap.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qdir.h>
#ifdef USEQPE
#include <qpe/global.h>
#endif
#include "CDrawBuffer.h"
#include "striphtml.h"
#include "hrule.h"

#include <qregexp.h>
#include <qimage.h>
#include <qpixmap.h>
//#include <qprogressdialog.h>
//#include <qapplication.h>

static unsigned char h2i(unsigned char c)
{
  unsigned char ret = 0;
  if ('0' <= c && c <= '9')
    {
      ret = c - '0';
    }
  else if ('a' <= c && c <= 'f')
    {
      ret = c - 'a' + 10;
    }
  return ret;
}

static void parse_color(const QString& attr, unsigned char& r, unsigned char& g, unsigned char& b)
{
  r = g = b = 0;
  if (attr.length() >= 7 && attr[0] == '#')
    {
      r = h2i(attr[1].unicode());
      r = 16*r + h2i(attr[2].unicode());
      g = h2i(attr[3].unicode());
      g = 16*g + h2i(attr[4].unicode());
      b = h2i(attr[5].unicode());
      b = 16*b + h2i(attr[6].unicode());
    }
  else if (attr == "red")
    {
      r = 255;
    }
  else if (attr == "green")
    {
      g = 255;
    }
  else if (attr == "blue")
    {
      b = 255;
    }
  else if (attr == "white")
    {
      r = g = b = 255;
    }
  else if (attr == "black")
    {
      r = g = b = 0;
    }
  else
    {
      qDebug("Don't understand colour \"%s\"", (const char*)attr);
    }
}

CNavigation_base<htmlmark> striphtml::m_nav;

void striphtml::skipblock(const QString& _ent)
{
  tchar ch = '>';
  CStyle dummy;
  QString ent;
  unsigned long pos;
  do 
    {
      while (ch != '<' && ch != UEOF)
	{
	  mygetch(ch, dummy, pos);
	}
      
      ch = skip_ws();
      
      ent = getname(ch, " >").lower();
      qDebug("Skipblock:%s", (const char*)ent);
    } while (ent != _ent && ch != UEOF);
}

void striphtml::reset()
{
  m_inblock = false;
  text_q = "";
  q = "";
  tablenesteddepth = 0;
  forcecentre = false;
  ignorespace = false;
  indent = 0;
  while (!stylestack.isEmpty()) stylestack.pop();
  currentstyle.unset();
}

void striphtml::locate(unsigned int n)
{
  qDebug("striphtml:locating:%u", n);
  reset();
  parent->locate(n);
}

int striphtml::getpara(CBuffer& buff, unsigned long& startpos)
{
  tchar ch;
  CStyle sty;
  unsigned long pos;
  int i = 0;
  parent->getch(ch, sty, startpos);
  pos = startpos;
  while (1)
    {
      if (ch == 10 && !isPre)
	{
	  ch = ' ';
	}
      if (ch == UEOF)
	{
	  //	  qDebug("EOF:%d:%u", i, pos);
	  buff[i] = 0;
	  if (i == 0)
	    {
	      i = -1;
	    }
	  return i;
	}
      else if (ch == '<')
	{
	  tchar ch2 = skip_ws();
	  QString ent = getname(ch2, " >");
	  ent = ent.lower();
	  //	  qDebug("ent:%s", (const char*)ent);
	  if (ent == "a")
	    {
	      buff[i++] = '<';
	      buff[i++] = 'a';
	      buff[i++] = ch2;
	      //	      buff[i] = 0; qDebug("ANCHOR:%s", (const char*)toQString(buff.data()));
	    }
	  else if (ent == "/a")
	    {
	      buff[i++] = '<';
	      buff[i++] = '/';
	      buff[i++] = 'a';
	      buff[i++] = ch2;
	      //	      buff[i] = 0; qDebug("/ANCHOR:%s", (const char*)toQString(buff.data()));
	    }
	  else if (ent == "div")
	    {
	      //	      buff[i] = 0; qDebug("DIV:%s", (const char*)toQString(buff.data()));
	      if (i == 0)
		{
		  buff[i++] = '<';
		  buff[i++] = 'd';
		  buff[i++] = 'i';
		  buff[i++] = 'v';
		  buff[i++] = ' ';
		  buff[i++] = ch2;
		  while (ch2 != '>' && ch2 != UEOF && i < 2048)
		    {
		      parent->getch(ch2, sty, pos);
		      buff[i++] = ch2;
		    }
		}
	      else
		{
		  locate(pos);
		}
	      buff[i++] = 0;
	      //	      qDebug("DIV:%s", (const char*)toQString(buff.data()));
	      return i;
	    }
	  else if (ent == "p" || (ent[0] == 'h' && ent.length() == 2 && QString("123456789").find(ent[1]) != -1))
	    {
	      buff[i++] = 0;
	      while (ch2 != '>' && ch2 != UEOF)
		{
		  parent->getch(ch2, sty, pos);
		}
	      return i;
	    }
	  else
	    {
	      while (ch2 != '>' && ch2 != UEOF)
		{
		  parent->getch(ch2, sty, pos);
		}
	    }
	}
      else
	{
	  buff[i++] = ch;
	}
      parent->getch(ch, sty, pos);
    }
}

QString striphtml::dehtml(const QString& _info)
{
  QString info;
  for (int i = 0; i < _info.length(); i++)
  {
    tchar ch = _info[i];
    if (ch == '%')
      {
	ch = 0;
	for (int j = 0; j < 2; j++)
	  {
	    ch <<= 4;
	    tchar ch1 = _info[++i];
	    if ('0' <= ch1 && ch1 <= '9')
	      {
		ch += ch1 - '0';
	      }
	    else if ('a' <= ch1 && ch1 <= 'f')
	      {
		ch += ch1 - 'a' + 10;
	      }
	    else if ('A' <= ch1 && ch1 <= 'F')
	      {
		ch += ch1 - 'A' + 10;
	      }
	  }
      }
    info += ch;
  }
  return info;
}

bool striphtml::findanchor(const QString& _info)
{
  //  QProgressDialog dlg("Finding link...", QString::null, 0, NULL, "progress", true);
  //  QProgressBar dlg(0);
  if (parent->findanchor(_info))
    {
      reset();
      return true;
    }
  qDebug("Using html find");
  parent->locate(parent->startSection());
#if defined(USEQPE) || defined(_WINDOWS) 
  QString info;
  for (int i = 0; i < _info.length(); i++)
  {
    tchar ch = _info[i];
    if (QString(".^$[]*+?").find(ch) != -1)
      {
	info += '\\';
      }
    info += ch;
  }
#else
  QString info = QRegExp::escape(_info);
#endif
  qDebug("Adjusted searchstring:%s", (const char*)info);
  QString sname("<[Aa][^>]*[ \t]+[Nn][Aa][Mm][Ee][ \t]*=[ \t]*\"?");
  sname += info + "\"?[ \t>]";
  QString sid("<[A-Za-z][^>]*[ \t]+[Ii][Dd][ \t]*=[ \t]*\"?");
  sid += info+"\"?[ \t>]";
#ifdef USEQPE
  QRegExp name(sname);
  QRegExp id(sid);
#else
  QRegExp name(sname+"|"+sid);
#endif
  bool ret = true;
  locate(0);
  unsigned long pos = 0;
  unsigned long startpos = 0;
  int offset;
  CBuffer test;
  qDebug("striphtml::findanchor");
  //  dlg.show();
  if (getpara(test, pos) >= 0)
    {
      while (1)
	{
	  //	  qApp->processEvents();
	  if ((offset = name.match(toQString(test.data()))) != -1) break;
#ifdef USEQPE
	  if ((offset = id.match(toQString(test.data()))) != -1) break;
#endif
	  if (getpara(test, pos) < 0)
	    {
	      locate(startpos);
	      qDebug("Not found");
	      return false;
	    }
	}
      locate(pos);
      qDebug("Found");
      ret = true;
    }
  else
    {
      locate(startpos);
      qDebug("Not found");
      ret = false;
    }
  return ret;
}

striphtml::striphtml(const QString& _s) : entmap(NULL), isPre(false), currentid(0), lastch(0), currentfile(_s), indent(0), forcecentre(false), m_inblock(false), m_bchm(false), ignorespace(false), tablenesteddepth(0)
{
  href2filepos = new QMap<QString, unsigned long>;
  id2href = new QMap<unsigned long, QString>;
}

striphtml::~striphtml()
{
  if (entmap != NULL) delete entmap;
  delete href2filepos;
  delete id2href;
}

void striphtml::initentmap()
{
  entmap = new QMap<QString, tchar>;
#ifdef USEQPE
#ifdef OPIE
  QString fname(getenv("OPIEDIR"));
#else
  QString fname(getenv("QTDIR"));
#endif
  fname += "/plugins/reader/data";
#else
  QString fname(getenv("READERDIR"));
  fname += "/data";
#endif  
  QFileInfo fi;
  fi.setFile(fname, "HTMLentities");
  if (fi.exists())
    {
      fname = fi.absFilePath();

      QFile fl(fname);
      if (fl.open(IO_ReadOnly))
	{
	  QTextStream t(&fl);
	  QString key, value;
	  while (!t.eof())
	    {
	      QString data = t.readLine();
	      int colon = data.find(':');
	      if (colon > 0)
		{
		  QString key = data.left(colon);
		  QString value = data.right(data.length()-colon-1);
		  bool ok;
		  int ret = value.toInt(&ok);
		  if (ok)
		    {
		      (*entmap)[key] = ret;
		    }
		}
	    }
	  fl.close();
	}
    }
}

unsigned short striphtml::skip_ws()
{
    tchar ch;
    CStyle sty;
    unsigned long dummy;
    do
    {
	mygetch(ch, sty, dummy);
    }
    while (ch < 33 && ch != UEOF);
    return ch;
}

unsigned short striphtml::skip_ws_end()
{
    unsigned long dummy;
    return skip_ws_end(dummy);
}

unsigned short striphtml::skip_ws_end(unsigned long& pos)
{
    tchar ch;
    CStyle sty;
    do
      {
	mygetch(ch, sty, pos);
      }
    while (ch != '>' && ch != UEOF);
    return ch;
}

QString striphtml::getname(tchar& ch, const QString& nd)
{
  QString nm = "";
  //  nm += ch;
  CStyle sty;
  unsigned long dummy;
  while (1)
    {
      //      if ( QChar(ch).isLetterOrNumber() )
      if (ch != UEOF && nd.find(ch, 0, false) == -1 && nm.length() < 2048)
	{
	  nm += ch;
	}
      else
	{
	  break;
	}
      mygetch(ch, sty, dummy);
    }
  return nm;
}

QString striphtml::getattr(tchar& ch)
{
  QString ref;
  CStyle sty;
  unsigned long pos;
  if (ch == ' ') ch = skip_ws();
  if (ch == '=')
    {
      ch = skip_ws();
      if (ch == '"')
	{
	  mygetch(ch, sty, pos);
	  ref = getname(ch, "\"");
	  ch = skip_ws();
	}
      else if (ch == '\'')
	{
	  mygetch(ch, sty, pos);
	  ref = getname(ch, "\'");
	  ch = skip_ws();
	}
      else
	{
	  ref = getname(ch, " >");
	  if (ch == ' ') ch = skip_ws();
	}
    }
  return ref;
}

linkType striphtml::hyperlink(unsigned int n, unsigned int, QString& w, QString& nm)
{
#if defined(USEQPE) || defined(_WINDOWS)
  QMap<unsigned long, QString>::Iterator hrefit = id2href->find(n);
#else
  QMap<unsigned long, QString>::iterator hrefit = id2href->find(n);
#endif
  if (hrefit == id2href->end())
    {
      return eNone;
    }
  QString href = *hrefit;
#if defined(USEQPE) || defined(_WINDOWS)
  QMap<QString, unsigned long>::Iterator fpit = href2filepos->find(href);
#else
  QMap<QString, unsigned long>::iterator fpit = href2filepos->find(href);
#endif
  if (fpit == href2filepos->end())
    {
      if (href == "history.back()")
	{
	  QString fc = currentfile;
	  unsigned long loc;
	  htmlmark m(fc, loc);
	  linkType ret = (m_nav.back(m)) ? eFile : eNone;
	  if (fc == m.filename())
	    {
	      if ((ret & eFile) != 0)
		{
		  locate(m.posn());
		  return eLink;
		}
	    }
	  return eNone;
	}
      qDebug("Searching for %s", (const char*)href);


      QString file, name;

      int colon = href.find('#');
      if (colon >= 0)
	{
	  file = dehtml(href.left(colon));
	  name = dehtml(href.right(href.length()-colon-1));
	}
      else
	{
	  file = dehtml(href);
	}
      
      qDebug("File:%s", (const char*)file);
      qDebug("Name:%s", (const char*)name);
      

      if (file.isEmpty())
	{
	  if (parent->findanchor(name))
	    {
	      reset();
	      return eLink;
	    }
	  fpit = href2filepos->find(name);
	  if (fpit != href2filepos->end())
	    {
	      locate(*fpit);
	      return eLink;
	    }
	  else
	    {
	      //	      nm = QString("<a[^>]*name[ \t]*=[ \t]*\"") + name + "\"";
	      qDebug("Do a search for:%s", (const char*)name);
	      findanchor(name);
	      return eLink;
	    }
	}
      else
	//      if (href.find('#') == -1)
	{
	  if (m_bchm)
	    {
	      w = file;
	      nm = name;
	      return eFile;
	    }
	  else
	    {
	      QFileInfo f(currentfile);
	      QFileInfo f1(f.dir(true), file);
	      if (f1.exists())
		{
		  w = f1.absFilePath();
		  nm = name;
		}
	      else
		{
		  w = file;
		}
	      return (f1.exists() ? eFile : eNone);
	    }
	}
      return eNone;
    }
  locate(*fpit);
  //  parent->locate((*href2filepos)[(*id2href)[n]]);
  return eLink;
}
/*
unsigned short striphtml::parse_m()
{
    tchar ch;
    CStyle sty;
    unsigned long dummy;
    mygetch(ch, sty, dummy);
    if (ch == 'm' || ch == 'M')
    {
	ch = skip_ws_end();
	if (ch == '>')
	{
	    return 0;
	}
    }
    return ch;
}
*/

void striphtml::mygetch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  if (!text_q.isEmpty() && !m_inblock)
    {
      ch = text_q[0].unicode();
      text_q = text_q.right(text_q.length()-1);
    }
  else
    {
      parent->getch(ch, sty, pos);
      if (ch == '<')
	{
	  m_inblock = true;
	}
      if (ch == '>')
	{
	  m_inblock = false;
	}
    }
  if (ch == 10 && !isPre)
    {
#ifdef REMOVE_LF_BEFORE_ENDTAG
      parent->getch(ch, sty, pos);
      if (ch == '<')
	{
	  parent->getch(ch, sty, pos);
	  if (ch == '/')
	    {
	      ch = '<';
	      text_q += '/';
	    }
	  else
	    {
	      text_q += '<';
	      text_q += ch;
	      ch = ' ';
	    }
	}
      else
	{
	  text_q += ch;
	  ch = ' ';
	}
#else
      ch = ' ';
#endif
    }
}

void striphtml::parse_paragraph(CStyle& currentstyle, tchar& ch, unsigned long pos)
{
/*
 int count = 0;
   for (CList<CStyle>::iterator iter = stylestack.begin(); iter != stylestack.end(); ++iter)
     {
	count++;
     }
   qDebug("Currently have %u styles", count);
*/
   if (stylestack.isEmpty())
     {
	currentstyle.unset();
     }
   else
     {
	currentstyle = stylestack.first();
     }
  if (forcecentre)
    {
      currentstyle.setCentreJustify();
    }
  if (ch == ' ') ch = skip_ws();
  while (ch != '>' && ch != UEOF)
    {
      QString ent = getname(ch, " =>").lower();
      QString attr = getattr(ch).lower();
      //qDebug("(Paragraph)Entity:%s Attr:%s", (const char*)ent, (const char*)attr);
      if (ent == "align")
	{
	  if (attr == "center")
	    {
	      currentstyle.setCentreJustify();
	    }
	  if (attr == "right")
	    {
	      currentstyle.setRightJustify();
	    }
	  if (attr == "justify")
	    {
	      currentstyle.setFullJustify();
	    }
	}
      if (ent == "id")
	{
	  (*href2filepos)[attr] = pos;
	}
      if (ent == "bgcolor")
	{
	  qDebug("Got paper colour:%s", (const char*)attr);
	  unsigned char r,g,b;
	  parse_color(attr, r, g, b);
	  currentstyle.setPaper(r, g, b);
	}
      if (ent == "color")
	{
	  qDebug("Got foreground colour:%s", (const char*)attr);
	  unsigned char r,g,b;
	  parse_color(attr, r, g, b);
	  currentstyle.setColour(r, g, b);
	}
      if (ch == ' ') ch = skip_ws();
    }
  ch = 10;
}

void striphtml::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  currentstyle.clearPicture();
  if (!q.isEmpty())
    {
      ch = q[0].unicode();
      if (ch == '-')
	{
	  tchar w = q[1].unicode();
	  tchar h = q[2].unicode();
	  unsigned char r = q[3].unicode();
	  unsigned char g = q[4].unicode();
	  unsigned char b = q[5].unicode();
	  ch = '#';
	  //qDebug("html:hrule<%u, %u>", w, h);
	  currentstyle.setPicture(false, hRule(w,h,r,g,b));
	  q = q.right(q.length()-6);
	}
      else
	{
	  q = q.right(q.length()-1);
	}
      sty = currentstyle;
      lastch = ch;
      return;
    }
  do
    {
      unsigned long npos;
      CStyle dummy;
      mygetch(ch, dummy, pos);
      while (ch == '<' && ch != UEOF)
	{
	  ch = skip_ws();
	  QString ent = getname(ch, " >").lower();

	  //	  qDebug("Entity:%s", (const char*)ent);
	
	  if (ent == "a"/* || ent == "reference"*/)
	    {
	      if (ch == ' ') ch = skip_ws();
	      bool fileposfound = false;
	      bool ishref = false;
	      unsigned int filepos = 0;
	      QString ref, name;
	      while (ch != '>' && ch != UEOF)
		{
		  QString ent = getname(ch, " =>").lower();
		  QString attr = getattr(ch);
		  //qDebug("<A>Entity:%s Attr:%s", (const char*)ent, (const char*)attr);
		  if (ent == "name")
		    {
		      name = attr;
		    }
		   if (ent == "onclick")
		     {
			int st = attr.find('\'');
			int nd = attr.findRev('\'');
			ref = attr.mid(st+1, nd-st-1);
			ishref = true;
			qDebug("Onclick:%s", (const char*)ref);
		     }
		  if (ent == "href")
		    {
		      ishref = true;
		      ref = attr;
		    }
		  if (ent == "filepos")
		    {
		      filepos = attr.toUInt(&fileposfound);
		      if (ref.isEmpty())
			{
			  ishref = true;
			  ref = attr;
			}
		    }
		  if (ent == "title")
		    {
		      text_q = attr + "</a><p>";
		    }
		  //qDebug("<a %s=%s>", (const char*)ent, (const char*)ref);
		}
	      if (ishref)
		{
		  currentstyle.setColour(0,0,255);
		  currentstyle.setLink(true);
		  currentstyle.setData(currentid);
		  if (!text_q.isEmpty())
		    {
		      currentstyle.setBold();
		      currentstyle.setCentreJustify();
		    }
		  (*id2href)[currentid] = ref;
		  currentid++;
		
		
		  if (fileposfound)
		    {
		      (*href2filepos)[ref] = filepos;
		    }
		}
	      if (!name.isEmpty())
		{
		  (*href2filepos)[name] = pos;
		}
	    }
	  else if (ent == "p")
	    {
	      parse_paragraph(currentstyle, ch, pos);
	      currentstyle.setExtraSpace(3);
	      continue;
	    }
	  else if (ent == "div")
	    {
	      parse_paragraph(currentstyle, ch, pos);
	      stylestack.push_front(currentstyle);
	      currentstyle.setExtraSpace(16);
		  //indent = 0;
	      continue;
	    }
	  else if (ent == "sup")
	    {
	      currentstyle.setVOffset(-1);
	    }
	  else if (ent == "sup")
	    {
	      currentstyle.setVOffset(1);
	    }
	  else if (ent == "/sup" || ent == "/sub")
	    {
	      currentstyle.setVOffset(0);
	    }
	  else if (ent == "span")
	    {
	      if (ch == ' ') ch = skip_ws();
	      while (ch != '>' && ch != UEOF)
		{
		  QString ent = getname(ch, " =>").lower();
		  QString attr = getattr(ch).lower();
		  if (ent == "bgcolor")
		    {
		      qDebug("Got background colour:%s", (const char*)attr);
		      unsigned char r,g,b;
		      parse_color(attr, r, g, b);
		      currentstyle.setBackground(r, g, b);
		    }
		  if (ent == "color")
		    {
		      qDebug("Got foreground colour:%s", (const char*)attr);
		      unsigned char r,g,b;
		      parse_color(attr, r, g, b);
		      currentstyle.setColour(r, g, b);
		    }
		}
	       stylestack.push_front(currentstyle);
	    }
	  else if (ent == "/span")
	    {
	      if (ch != '>') ch = skip_ws_end();
	      currentstyle.setBackground(255, 255, 255);
	      currentstyle.setColour(0, 0, 0);
	       if (!stylestack.isEmpty())
		 {
		    stylestack.pop();
		 }
	    }
	  else if (ent == "pre")
	    {
	      isPre = true;
	      currentstyle.setNoJustify();
	      currentstyle.setMono();
	    }
	  else if (ent == "tt")
	    {
	      currentstyle.setMono();
	    }
	  else if (ent == "b" || ent == "strong")
	    {
	      currentstyle.setBold();
	    }
	  else if (ent == "u")
	    {
	      currentstyle.setUnderline();
	    }
	  else if (ent == "/u")
	    {
	      currentstyle.unsetUnderline();
	    }
	  else if (ent == "blockquote")
	    {
	      if (ch != '>') ch = skip_ws_end();
	      ch = 10;
	      currentstyle.setExtraSpace(0);
	      currentstyle.setLeftMargin(30);
	      currentstyle.setRightMargin(30);
	      continue;
	    }
	  else if (ent == "br" || ent == "br/")
	    {
	      if (ch != '>') ch = skip_ws_end();
	      ch = 10;
	      currentstyle.setExtraSpace(0);
	      lastch = 0;
	      continue;
	    }
	  else if (ent == "mbp:pagebreak")
	    {
	      /*
	      if (ch != '>') ch = skip_ws_end(pos);
	      q += 10;
	      q += QChar(UEOF);
	      ch = 10;
	      continue;
	      */
	      ch = 6;
	      //	      currentstyle.setTop();
	      continue;
	    }
	  else if (ent == "center")
	    {
	      //forcecentre = true;
	      qDebug("setting centre");
	      currentstyle.setCentreJustify();
	       ch = 10;
	       continue;
	    }
	  else if (ent == "/center")
	    {
	      qDebug("unsetting centre");
	      forcecentre = false;
	    }
	  else if (ent == "li")
	    {
	      if (ch != '>') ch = skip_ws_end();
	      lastch = 0;
	      ch = 10;
	      if (m_listtype[indent % m_cmaxdepth] == 1)
		{
		  q.setNum(m_ctr[indent % m_cmaxdepth]++);
		}
	      else
		{
		  q += QChar(8226);
		}
	      q += ' ';
	      currentstyle.setLeftMargin(6*indent);
	      qDebug("Setting indent:%d", indent);
	      continue;
	    }
	  else if (ent == "ul")
	    {
	      indent++;
	      m_listtype[indent % m_cmaxdepth] = 0;
	    }
	  else if (ent == "/ul")
	    {
	      indent--;
	    }
	  else if (ent == "ol")
	    {
	      indent++;
	      m_listtype[indent % m_cmaxdepth] = 1;
	      m_ctr[indent % m_cmaxdepth] = 1;
	    }
	  else if (ent == "/ol")
	    {
	      indent--;
	    }
	  else if (ent == "i")
	    {
	      currentstyle.setItalic();
	    }
	  else if (ent == "em")
	    {
	      currentstyle.setItalic();
	    }
	  else if (ent == "small")
	    {
	      currentstyle.setFontSize(-2);
	    }
	  else if (ent == "/small")
	    {
	      currentstyle.setFontSize(0);
	    }
	  else if (ent == "big")
	    {
	      currentstyle.setFontSize(2);
	    }
	  else if (ent == "/big")
	    {
	      currentstyle.setFontSize(0);
	    }
	  else if (ent[0] == '/' && ent[1] == 'h' && ent.length() == 3 && QString("123456789").find(ent[2]) != -1)
	    {
	      parse_paragraph(currentstyle, ch, pos);
	      currentstyle.setExtraSpace(3);
	      continue;
	    }
	  else if (ent[0] == 'h' && ent.length() == 2 && QString("123456789").find(ent[1]) != -1)
	    {
	      indent = 0;
	      if (ent[1] == '1')
		{
		  parse_paragraph(currentstyle, ch, pos);
		  currentstyle.setFontSize(3);
		  currentstyle.setExtraSpace(8);
		  currentstyle.setBold();
		  //		    currentstyle.setExtraSpace(10);
		}
	      else if (ent[1] == '2')
		{
		  parse_paragraph(currentstyle, ch, pos);
		  currentstyle.setFontSize(2);
		  currentstyle.setExtraSpace(6);
		  currentstyle.setBold();
		  //		    currentstyle.setExtraSpace(10);
		}
	      else if (ent[1] == '3')
		{
		  parse_paragraph(currentstyle, ch, pos);
		  currentstyle.setFontSize(1);
		  currentstyle.setExtraSpace(4);
		  currentstyle.setBold();
		  //		    currentstyle.setExtraSpace(10);
		}
	      else
		{
		  parse_paragraph(currentstyle, ch, pos);
		  currentstyle.setExtraSpace(4);
		  currentstyle.setBold();
		  //		    currentstyle.setExtraSpace(10);
		}
	      ch = 10;
	      continue;
	    }
	
	
	  else if (ent == "/a")
	    {
	      currentstyle.setColour(0,0,0);
	      currentstyle.setLink(false);
	    }
	  else if (ent == "/pre")
	    {
	      currentstyle.unsetMono();
	      isPre = false;
	    }
	  else if (ent == "/tt")
	    {
	      currentstyle.unsetMono();
	    }
	  else if (ent == "/b" || ent == "/strong")
	    {
	      currentstyle.unsetBold();
	    }
	  else if (ent == "/i")
	    {
	      currentstyle.unsetItalic();
	    }
	  else if (ent == "/em")
	    {
	      currentstyle.unsetItalic();
	    }
	  else if (ent == "/div")
	    {
	      currentstyle.unset();
	      if (ch != '>') ch = skip_ws_end();
	      ch = 10;
	       if (!stylestack.isEmpty())
		 {
		    stylestack.pop();
		 }
	      continue;
	    }
	  else if (ent == "tr")
	    {
	      if (ch != '>') ch = skip_ws_end();
	      ch = 10;
	      q += '-';
	      q += QChar(parent->getwidth());
	      q += 2;
	      q += '\0';
	      q += '\0';
	      q += '\0';
	      continue;
	    }
	  else if (ent == "td")
	    {
	      if (ch != '>') ch = skip_ws_end();
	      ignorespace = false;
	    }
	  else if (ent == "/td")
	    {
	      ignorespace = true;
	      //	      parse_paragraph(currentstyle, ch, pos);
	      //stylestack.push_front(currentstyle);
	      if (ch != '>') ch = skip_ws_end();
	      //	      ch = '|';
	      //continue;
	      ch = 10;
	      q += '-';
	      q += QChar(parent->getwidth());
	      q += 1;
	      q += '\0';
	      q += '\0';
	      q += '\0';
	      continue;
	    }
	  /*
	  else if (ent == "/td")
	    {
	      currentstyle.unset();
	      if (ch != '>') ch = skip_ws_end();
	       if (!stylestack.isEmpty())
		 {
		    stylestack.pop();
		 }
	       //	       ch = 10;
	       continue;
	    }
	  */
	  else if (ent[0] == '/' && ent.length() == 3 && ent[1] == 'h' && QString("123456789").find(ent[2]) != -1)
	    {
	      currentstyle.unset();
	      if (ch != '>') ch = skip_ws_end();
	      //ch = 10;
	      //continue;
	    }
	  else if (ent == "table" || ent == "/table")
	    {
	      currentstyle.unset();
	      ignorespace = (ent == "table");
	      if (ent == "table")
		{
		  if (tablenesteddepth++ == 0) currentstyle.setTable(pos);
		}
	      else
		{
		  if (--tablenesteddepth <= 0)
		    {
		      tablenesteddepth = 0;
		      currentstyle.setTable(0xffffffff);
		    }
		}
	      if (ch == ' ') ch = skip_ws();
	      while (ch != '>' && ch != UEOF)
		{
		  QString ent = getname(ch, " =>").lower();
		  QString attr = getattr(ch);
		  qDebug("<table>Entity:%s Attr:%s", (const char*)ent, (const char*)attr);
		}
	      if (ch != '>') ch = skip_ws_end();

	      currentstyle.setLeftMargin(6*tablenesteddepth);


	      lastch = 0; // Anything but 10
	      ch = 10;
	      q += '-';
	      q += QChar(parent->getwidth());
	      q += 3;
	      q += '\0';
	      q += '\0';
	      q += '\0';
	      continue;
	    }
	  else if (ent == "hr")
	    {
	      //bool isPageBreak = false;
	      if (ch == ' ') ch = skip_ws();
	      unsigned char red = 0, green = 0, blue = 0;
	      while (ch != '>' && ch != UEOF)
		{
		  QString ent = getname(ch, " =>").lower();
		  QString attr = getattr(ch);
		  if (ent == "color")
		    {
		      parse_color(attr, red, green, blue);
		    }
		  /*
		  if (ent == "size")
		    {
		      if (attr == "0")
			{
			  isPageBreak = true;
			}
		    }
		  */
		  qDebug("<hr>Entity:%s Attr:%s", (const char*)ent, (const char*)attr);
		}
	      if (ch != '>') ch = skip_ws_end();
	      /*
	      if (isPageBreak)
		{
		  ch = UEOF;
		}
	      else
		{
	      */
		  //	      if (stylestack.isEmpty())
		  //		{
		  currentstyle.unset();
		  //		}
		  /*
		    else
		    {
		    qDebug("Using stack style");
		    currentstyle = stylestack.first();
		    }
		  */
		  lastch = 0; //Anything but 10 or ' '
		  ch = 10;
		  q += '-';
		  q += QChar(parent->getwidth());
		  q += 3;
		  q += red;
		  q += green;
		  q += blue;

	      continue;
	    }



	  else if (ent == "img")
	    {
	      if (ch == ' ') ch = skip_ws();
	      while (ch != '>' && ch != UEOF)
		{
		  QString ent = getname(ch, " =>").lower();
		  QString attr = getattr(ch);
		  qDebug("<img>Entity:%s Attr:%s", (const char*)ent, (const char*)attr);
		  if (ent == "src")
		    {
		      /*
		      if (m_bchm)
			{
			  QImage* img = parent->getPicture(attr);
			  if (img != NULL)
			    {
			      currentstyle.setPicture(true, img);
			    }
			}
		      */


		      QImage* img = parent->getPicture(attr);
		      if (img != NULL)
			{
			  currentstyle.setPicture(true, img);
			}
		      else
			{
			  QFileInfo f(currentfile);
			  QFileInfo f1(f.dir(true), attr);
			  QPixmap pm;
			  if (pm.load(f1.absFilePath()))
			    {
			      QImage* img = new QImage(pm.convertToImage());
			      currentstyle.setPicture(true, img);
			    }
			}
		    }
		  if (ent == "recindex")
		    {
		      bool ok;
		      unsigned int picindex = attr.toUInt(&ok);
                      qDebug("Looking for image at %u", picindex);
                      QImage* img = parent->getPicture(picindex);
                      if (img != NULL)
			{
			  currentstyle.setPicture(true, img);
			}
                      else
			{
			  qDebug("No image found");
			}
		    }
		}
	      if (ch != '>') ch = skip_ws_end();
	      ch = '#';
	      break;
	    }
	  else if (ent.left(2) == "dc")
	    {
	      QString nd("/");
	      skipblock(nd+ent);
	    }
	  else if (ent == "metadata")
	    {
	      //	      skipblock("/metadata");
	    }
	  else if (ent == "title")
	    {
	      skipblock("/title");
	    }
	  else if (ent == "head")
	    {
	      skipblock("/head");
	    }
	  /*
	  else if (ent == "metadata")
	    {
	      currentstyle.setFontSize(-2);
	    }
	  else if (ent == "/metadata")
	    {
	      currentstyle.unset();
	      ch = 10;
	      continue;
	    }
	  */
	  else
	    {
	      if (ent[0] != '/')
		qDebug("Not handling:%s", (const char*)ent);
	    }

	  if (ch != '>') ch = skip_ws_end();
	  if (ent[0] == '/')
	    mygetch(ch, dummy, pos);
	  else
	    mygetch(ch, dummy, npos);
	}
      if (ch == '&')
	{
	  mygetch(ch, dummy, npos);
	  if (ch == '#')
	    {
	      int id = 0;
	      mygetch(ch, dummy, npos);
	      while (ch != ';' && ch != UEOF)
		{
		  id = 10*id+ch-'0';
		  mygetch(ch, dummy, npos);
		}
	      ch = id;
	    }
	  else
	    {
	      QString en;
	      en += ch;
	      mygetch(ch, dummy, npos);
	      while (ch != ';' && ch != UEOF)
		{
		  en += ch;
		  mygetch(ch, dummy, npos);
		}
	      if (entmap == NULL) initentmap();
#if defined(USEQPE) || defined(_WINDOWS)
	      QMap<QString, tchar>::Iterator it = entmap->find(en);
#else
	      QMap<QString, tchar>::iterator it = entmap->find(en);
#endif
	      if (it != entmap->end())
		{
		  ch = *it;
		}
	      else
		{
		  ch = '.';
		}
	    }
	}
      //    sty = (dummy == ucFontBase) ? currentstyle : dummy;
      if (lastch == 10 && ch == 10 && sty.getExtraSpace() > currentstyle.getExtraSpace())
	{
	  currentstyle.setExtraSpace(sty.getExtraSpace());
	}
      sty = currentstyle;
    }
  while (!isPre && (((lastch == ' '  || lastch == 10 || ignorespace) && ch == ' ') || ((ch == 10) && (lastch == 10))));
  //  lastch = ch;
  lastch = ch;
  return;
}

QString striphtml::getTableAsHtml(unsigned long loc)
{
  qDebug("striphtml::getTableAsHtml");
  QString ret;
  tchar ch(0);
  CStyle sty;
  unsigned long pos;
  locate(loc);
  int endpos(0);
  QString endmarker("</table>");
  QString startmarker("<table");
  int startpos(0);
  int depth(0);
  while (ch != UEOF)
    {
      parent->getch(ch, sty, pos);
      QChar qc(ch);
      ret += qc;
      if (qc.lower() == endmarker[endpos])
	{
	  if ((++endpos >= endmarker.length()) && (--depth <= 0)) break;
	}
      else
	{
	  endpos = 0;
	}
      if (qc.lower() == startmarker[startpos])
	{
	  if (++startpos >= startmarker.length()) ++depth;
	}
      else
	{
	  startpos = 0;
	}
    }
  return ret;
}


extern "C"
{
  CFilter* newfilter(const QString& s) { return new striphtml(s); }
}
