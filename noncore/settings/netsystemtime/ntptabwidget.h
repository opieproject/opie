/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
:     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef NTPTABWIDGET_H
#define NTPTABWIDGET_H

#include <qwidget.h>

class QLabel;
class QMultiLineEdit;
class QPushButton;

class NTPTabWidget : public QWidget
{
	Q_OBJECT

public:
	NTPTabWidget( QWidget * = 0x0 );
	~NTPTabWidget();

	void setStartTime( const QString & );
	void setTimeShift( const QString & );
	void setNewTime( const QString & );
	void addNtpOutput( const QString & );
    void setNTPBtnEnabled( bool enabled );

private:
	QLabel         *lblStartTime;
	QLabel         *lblTimeShift;
	QLabel         *lblNewTime;
	QMultiLineEdit *mleNtpOutput;
    QPushButton    *m_ntpBtn;


signals:
	void getNTPTime();
};

#endif
