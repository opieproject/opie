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

#ifndef __GUI_SETTINGS_H__
#define __GUI_SETTINGS_H__

#include <qwidget.h>
#include <qmap.h>

class QListView;
class QCheckListItem;
class QCheckBox;


class GuiSettings : public QWidget {
	Q_OBJECT

public:
	GuiSettings ( QWidget *parent = 0, const char *name = 0 );

	void accept ( );

protected slots:
	void menuChanged ( );
	void busyChanged ( );

protected:
	void init ( );

private:
	QCheckBox *m_omenu;
	QCheckBox *m_omenu_tabs;
	QCheckBox *m_omenu_home;
	QCheckBox *m_omenu_suspend;
	QCheckBox *m_busy;

	bool m_menu_changed;
	bool m_busy_changed;
};

#endif
