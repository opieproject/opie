#include "util.h"
#include <limits.h>

QString filesize(unsigned long l)
{
  char units[] = { 'K', 'M', 'G' };
  int iu = 0;
  while (l > 999 && iu < sizeof(units))
    {
      l = (l+512)/1024;
      iu++;
    }
  QString ret = QString::number(l);
  if (iu != 0)
    {
      ret += QChar(units[iu-1]);
    }
  return ret;
}

QString percent(unsigned long pos, unsigned long len)
{
  unsigned long permil = 0;
  if (len != 0)
    {
      if (pos > UINT_MAX/1000)
	{
	  unsigned long l1 = (len+500)/1000;
	  permil = (pos+l1/2)/l1;
	}
      else
	{
	  permil = (1000*pos+len/2)/len;
	}
    }
  unsigned long percen = permil/10;
  unsigned long frac = permil - 10*percen;
  return QString::number(percen) + "." + QString::number(frac);
}

QString fmt(unsigned long pos, unsigned long len)
{
  QString ret = filesize(pos) + "/" + filesize(len) + " : " + percent(pos,len) +"%";
  return ret;
}
