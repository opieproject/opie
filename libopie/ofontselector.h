/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#ifndef __OPIE_FONTSELECTOR_H__
#define __OPIE_FONTSELECTOR_H__

#include <qwidget.h>

class QListBox;
class OFontSelectorPrivate;


class OFontSelector : public QWidget
{
    Q_OBJECT

public:
    OFontSelector ( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    virtual ~OFontSelector ( );

	bool selectedFont ( QString &family, QString &style, int &size );
	bool selectedFont ( QString &family, QString &style, int &size, QString &charset );
	
	QFont selectedFont ( );

	bool setSelectedFont ( const QFont & );
	bool setSelectedFont ( const QString &family, const QString &style, int size, const QString &charset = 0 );

	QString fontFamily ( ) const;
	QString fontStyle ( ) const;
	int fontSize ( ) const;
	QString fontCharSet ( ) const;

signals:
	void fontSelected ( const QFont & );

protected slots:
    virtual void fontFamilyClicked ( int );
    virtual void fontStyleClicked ( int );
    virtual void fontSizeClicked ( int );

private:
	void loadFonts ( QListBox * );

    void changeFont ( );
	
private:
	OFontSelectorPrivate *d;
};

#endif

