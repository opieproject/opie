/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef __BUTTON_UTILS_H__
#define __BUTTON_UTILS_H__

#include <opie/odevicebutton.h>

class AppLnkSet;
class QListViewItem;


class qCopInfo {
public:
	qCopInfo ( const QString &str, const QPixmap &pix = QPixmap ( ))
		: m_name ( str ), m_icon ( pix )
	{ }

	QString m_name;
	QPixmap m_icon;
};


class ButtonUtils {
public:
	~ButtonUtils ( );

	static ButtonUtils *inst ( );
	
	qCopInfo messageToInfo ( const Opie::OQCopMessage & );

	void insertActions ( QListViewItem *here );
	void insertAppLnks ( QListViewItem *here );
		
private:
	ButtonUtils ( );
	static void cleanup ( );
	
private:
	AppLnkSet *m_apps;
};

#endif
