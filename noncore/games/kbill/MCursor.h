/***************************************************************************
                          MCursor.h  -  description
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
#ifndef MCURSOR_H
#define MCURSOR_H

/*#include <X11/Xlib.h>
#include <X11/xpm.h>*/

#include <qcursor.h>

class MCursor {
public:
	~MCursor();
	static const int SEP_MASK = 0;
	static const int OWN_MASK = 1;
	void load(const char *name, int masked);
private:
	QCursor *cursor;

	friend class UI;
};

#endif
