
#include <qimage.h>
#include "StyleConsts.h"

GraphicLink::~GraphicLink() { delete graphic; }

pmstore::~pmstore()
{ 
////    odebug << "Deleting image" << oendl; 
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
