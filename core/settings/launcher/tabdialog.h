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

#ifndef __TABDIALOG_H__
#define __TABDIALOG_H__

#include <qdialog.h>
#include "tabconfig.h"

class QButtonGroup;
class OFontSelector;
class SampleView;
class OColorButton;
class QPushButton;
class QCheckBox;


class TabDialog : public QDialog {
	Q_OBJECT
public:
	TabDialog ( const QPixmap *tabicon, const QString &tabname, TabConfig &cfg, QWidget *parent = 0, const char *dname = 0, bool modal = false, WFlags = 0 );
	virtual ~TabDialog ( );	

public slots:
	virtual void accept ( );

protected slots:
	void iconSizeClicked ( int );
	void fontClicked ( const QFont & );
	void bgTypeClicked ( int );
	void bgColorClicked ( const QColor & );
	void iconColorClicked ( const QColor & );
	void bgImageClicked ( );
	void bgDefaultClicked ( );

private:
	QWidget *createBgTab ( QWidget *parent );
	QWidget *createFontTab ( QWidget *parent );
	QWidget *createIconTab ( QWidget *parent );
	
	
private:
	SampleView *m_sample;
	QButtonGroup *m_iconsize;
	OFontSelector *m_fontselect;
	OColorButton *m_solidcolor;
	OColorButton *m_iconcolor;
	QPushButton *m_imagebrowse;
	QString m_bgimage;	
	QButtonGroup *m_bgtype;
	QCheckBox *m_fontuse;
	
	TabConfig &m_tc;
};


#endif
