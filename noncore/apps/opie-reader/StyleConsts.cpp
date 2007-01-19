
#include <qimage.h>
#include "StyleConsts.h"

GraphicLink::~GraphicLink() { delete graphic; }

pmstore::pmstore(bool _canScale, QImage* p, bool isLnk, unsigned long tgt) :
  count(1),
  m_isScaleable(_canScale)
{
    graphic = new GraphicLink(p, isLnk, tgt);
}

pmstore::~pmstore()
{ 
////    qDebug("Deleting image");
    delete graphic;
}

CStyle::~CStyle()
{
    if (graphic != NULL)
    {
	if (--(graphic->count) == 0)
	{
	    delete graphic;
	}
    }
}

CStyle::CStyle(const CStyle& rhs) : graphic(NULL)
{
    *this = rhs;
}

CStyle& CStyle::operator=(const CStyle& rhs)
{
    if (rhs.graphic != NULL)
    {
	(rhs.graphic->count)++;
	if (graphic != NULL)
	{
	    if (--(graphic->count) == 0)
	    {
		delete graphic;
	    }
	}
	graphic = rhs.graphic;
    }
    else
    {
	if (graphic != NULL)
	{
	    if (--(graphic->count) == 0)
	    {
		delete graphic;
	    }
	    graphic = NULL;
	}
    }
    sty = rhs.sty;
    return *this;
}

void CStyle::clearPicture()
{
    if (graphic != NULL)
    {
	if (--(graphic->count) == 0)
	{
	    delete graphic;
	}
	graphic = NULL;
    }
}

void CStyle::unset()
{
    sty.unset();
    if (graphic != NULL)
    {
	if (--(graphic->count) == 0)
	{
	    delete graphic;
	}
	graphic = NULL;
    }
}

void CStyle::setPicture(bool canScale, QImage* _g, bool il, unsigned long tgt)
{
    if (graphic != NULL)
    {
	if (--(graphic->count) == 0)
	{
	    delete graphic;
	}
	graphic = NULL;
    }
    if (_g != NULL) graphic = new pmstore(canScale, _g, il, tgt);
}

void CStyle::invert()
{
  qDebug("Before:<%02x%02x%02x>", sty.bred, sty.bgreen, sty.bblue);
  qDebug("Before:<%02x%02x%02x>", sty.red, sty.green, sty.blue);
  sty.bred = 255-sty.bred;
  sty.bgreen = 255-sty.bgreen;
  sty.bblue = 255-sty.bblue;
  sty.red = 255-sty.red;
  sty.green = 255-sty.green;
  sty.blue = 255-sty.blue;
  qDebug("After:<%02x%02x%02x>", sty.bred, sty.bgreen, sty.bblue);
  qDebug("After:<%02x%02x%02x>", sty.red, sty.green, sty.blue);
}
