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

#ifndef EXCEPTLISTITEM_H
#define EXCEPTLISTITEM_H

#include <qlistview.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qpainter.h>

class ExceptListItem : public QListViewItem {
public:
	ExceptListItem ( QListView *lv, const QString &t, bool nostyle = true, bool nofont = true, bool nodeco = true ) : QListViewItem ( lv )
	{
		m_text = t;
		
		m_nofont = nofont;
		m_nostyle = nostyle;
		m_nodeco = nodeco;
		
		init ( );
	}
	
	virtual ~ExceptListItem ( )
	{
	}
	
	static void overlay ( QImage &img, const QImage &ovl )
	{
		if (( img. size ( ) != ovl. size ( )) || 
		    ( img. depth ( ) != ovl. depth ( )))
			return;
		
		for ( int y = 0; y != img. height ( ); y++ ) {
			QRgb *iline = (QRgb *) img. scanLine ( y );
			QRgb *oline = (QRgb *) ovl. scanLine ( y );
			
			for ( int x = 0; x != img. width ( ); x++ ) {
				QRgb i = *iline;
				QRgb o = *oline;
			
				*iline = qRgba (( qRed ( i ) + qRed ( o )) / 2,
				                ( qGreen ( i ) + qGreen ( o )) / 2,
				                ( qBlue ( i ) + qBlue ( o )) / 2,
				                ( qAlpha ( i ) + qAlpha ( o )) / 2 );
				iline++;
				oline++;
			}
		}
	}
	
	static void init ( )
	{
		static bool init = false;
		
		if ( init ) 
			return;
	
		QImage noimg    = Resource::loadImage ( "appearance/no.png" );
		QImage fontimg  = Resource::loadImage ( "appearance/font.png" );
		QImage styleimg = Resource::loadImage ( "appearance/style.png" );
		QImage decoimg  = Resource::loadImage ( "appearance/deco.png" );

		s_fontpix [0] = new QPixmap ( );
		s_fontpix [0]-> convertFromImage ( fontimg );
		overlay ( fontimg, noimg );
		s_fontpix [1] = new QPixmap ( );
		s_fontpix [1]-> convertFromImage ( fontimg );

		s_stylepix [0] = new QPixmap ( );
		s_stylepix [0]-> convertFromImage ( styleimg );
		overlay ( styleimg, noimg );
		s_stylepix [1] = new QPixmap ( );
		s_stylepix [1]-> convertFromImage ( styleimg );

		s_decopix [0] = new QPixmap ( );
		s_decopix [0]-> convertFromImage ( decoimg );
		overlay ( decoimg, noimg );
		s_decopix [1] = new QPixmap ( );
		s_decopix [1]-> convertFromImage ( decoimg );

		init = true;
	}
	
	bool noFont ( ) const
	{
		return m_nofont;
	}

	bool noStyle ( ) const
	{
		return m_nostyle;
	}

	bool noDeco ( ) const
	{
		return m_nodeco;
	}

	void setNoDeco ( bool b )
	{
		if ( b != m_nodeco ) {
			m_nodeco = b;
			repaint ( );
		}
	}

	void setNoStyle ( bool b )
	{
		if ( b != m_nostyle ) {
			m_nostyle = b;
			repaint ( );
		}
	}

	void setNoFont ( bool b )
	{
		if ( b != m_nofont ) {
			m_nofont = b;
			repaint ( );
		}
	}
	
	QString pattern ( ) const 
	{
		return m_text;
	}
	
	void setPattern ( const QString &s )
	{
		if ( s != m_text ) {
			m_text = s;
			widthChanged ( 3 );
			repaint ( );
		}
	}

	QString text ( int i ) const
	{
		if ( i == 3 )
			return m_text;
		else
			return QString::null;
	
	}
	
	const QPixmap *pixmap ( int i ) const
	{
		if ( i == 0 )
			return (const QPixmap *) s_stylepix [m_nostyle ? 1 : 0];
		else if ( i == 1 )
			return (const QPixmap *) s_fontpix [m_nofont ? 1 : 0];
		else if ( i == 2 )
			return (const QPixmap *) s_decopix [m_nodeco ? 1 : 0];
		else
			return 0;
	}

private:
	QString m_text;
	bool m_nofont;
	bool m_nostyle;
	bool m_nodeco;
	
	static QPixmap *s_stylepix [2];
	static QPixmap *s_fontpix [2];
	static QPixmap *s_decopix [2];
};

QPixmap *ExceptListItem::s_stylepix [2] = { 0, 0 };
QPixmap *ExceptListItem::s_fontpix [2] = { 0, 0 };
QPixmap *ExceptListItem::s_decopix [2] = { 0, 0 };

#endif
