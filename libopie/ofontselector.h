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

#ifndef OPIE_FONTSELECTOR_H__
#define OPIE_FONTSELECTOR_H__

#include <qwidget.h>

class QListBox;
class OFontSelectorPrivate;

/**
 * This class lets you chose a Font out of a list of Fonts.
 * It can show a preview too. This selector will use all available
 * fonts
 *
 *
 * @short A widget to select a font
 * @see QWidget
 * @see QFont
 * @author Rober Griebl
 */
class OFontSelector : public QWidget
{
    Q_OBJECT

public:
    OFontSelector ( bool withpreview, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
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
        /**
         * This signal gets emitted when a font got chosen
         */
	void fontSelected ( const QFont & );

protected slots:
    /** @internal */
    virtual void fontFamilyClicked ( int );
    /** @internal */
    virtual void fontStyleClicked ( int );
    /** @internal */
    virtual void fontSizeClicked ( int );

protected:
	virtual void resizeEvent ( QResizeEvent *re );

private:
	void loadFonts ( QListBox * );

    void changeFont ( );

private:
	OFontSelectorPrivate *d;
};

#endif

