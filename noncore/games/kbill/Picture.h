/***************************************************************************
                          Picture.h  -  description
                             -------------------
    begin                : Thu Dec 30 1999
    copyright            : (C) 1999 by Jurrien Loonstra
    email                : j.h.loonstra@st.hanze.nl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PICTURE_H
#define PICTURE_H

#include <qpixmap.h>

class Picture {
public:	int width, height;
	void load(const char *name, int index=-1);
	QPixmap* getPixmap();
private:
	QPixmap *pix;

	friend class UI;
};

#endif
