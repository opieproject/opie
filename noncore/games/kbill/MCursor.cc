/***************************************************************************
                          MCursor.cc  -  description
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
#include "MCursor.h"
#include "objects.h"

#include <qcursor.h>
#include <qbitmap.h>
#include <qwidget.h>
#include <qstring.h>
#ifdef KDEVER
#include <kapp.h>
#include <kstandarddirs.h>
#endif
#include <iostream.h>
#include <qpe/resource.h>
MCursor::~MCursor() {
	delete cursor;
}

void MCursor::load(const char *name, int masked) {
	
	#ifdef KDEVER
	QString file, mfile;
        KStandardDirs dirs;


	file = dirs.findResource("data","kbill/bitmaps/" + QString::fromLocal8Bit(name)  + ".xbm");

	QBitmap bitmap, mask;
	if (bitmap.load(file) == FALSE) {
		cerr << "cannot open " << file << endl;
		exit(1);
	}
	if (masked == SEP_MASK) {
//		mfile.sprintf ("%sbitmaps/%s_mask.xbm", (const char*)dir, name);
		mfile = file = dirs.findResource("data","kbill/bitmaps/" + QString::fromLocal8Bit(name)  + "_mask.xbm");
		if (mask.load(mfile) == FALSE) {
			cerr << "cannot open " << file << endl;
			exit(1);
		}
	}
	else
	mask = bitmap;
	#endif

	QBitmap bitmap, mask;
	bitmap = Resource::loadBitmap("kbill/bitmaps/" + QString::fromLocal8Bit(name));

        if (masked == SEP_MASK)
	    mask = bitmap = Resource::loadBitmap("kbill/bitmaps/" + QString::fromLocal8Bit(name) + "_mask.xbm");
	else
	   mask = bitmap;
	cursor = new QCursor(bitmap, mask, bitmap.width() / 2, bitmap.height() / 2);
}

