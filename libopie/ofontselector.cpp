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

#include <qlayout.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qfont.h>
#include <qmultilineedit.h>

#include <qpe/fontdatabase.h>

#include "ofontselector.h"

class OFontSelectorPrivate {
public: 
	QListBox *      m_font_family_list;
	QComboBox *     m_font_style_list;
	QComboBox *     m_font_size_list;
	QMultiLineEdit *m_preview;

	bool            m_pointbug;

	FontDatabase    m_fdb;
};


class FontListItem : public QListBoxText {
public:
	FontListItem ( const QString &t, const QStringList &styles, const QValueList<int> &sizes ) : QListBoxText ( )
	{
		m_name = t;
		m_styles = styles;
		m_sizes = sizes;
		
		QString str = t;
		str [0] = str [0]. upper ( );
		setText ( str );
	}

	QString family ( ) const
	{
		return m_name;
	}
	
	const QStringList &styles ( ) const
	{
		return m_styles;
	}
	
	const QValueList<int> &sizes ( ) const
	{
		return m_sizes;
	}
	
private:
	QStringList m_styles;
	QValueList<int> m_sizes;
	QString m_name;
};


static int findItemCB ( QComboBox *box, const QString &str )
{
	for ( int i = 0; i < box-> count ( ); i++ ) {
		if ( box-> text ( i ) == str )
			return i;
	}
	return -1;
}


static int qt_version ( )
{
	const char *qver = qVersion ( );

	return ( qver [0] - '0' ) * 100 + ( qver [2] - '0' ) * 10 + ( qver [4] - '0' );
}


OFontSelector::OFontSelector ( bool withpreview, QWidget *parent, const char *name, WFlags fl ) : QWidget ( parent, name, fl )
{
	d = new OFontSelectorPrivate ( );

	QGridLayout *gridLayout = new QGridLayout ( this, 0, 0, 4, 4 );
	gridLayout->setRowStretch ( 4, 10 );

	d-> m_font_family_list = new QListBox( this, "FontListBox" );
	gridLayout->addMultiCellWidget( d-> m_font_family_list, 0, 4, 0, 0 );
	connect( d-> m_font_family_list, SIGNAL( highlighted( int ) ), this, SLOT( fontFamilyClicked( int ) ) );

	QLabel *label = new QLabel( tr( "Style" ), this );
	gridLayout->addWidget( label, 0, 1 );

	d-> m_font_style_list = new QComboBox( this, "StyleListBox" );
	connect( d-> m_font_style_list, SIGNAL(  activated( int ) ), this, SLOT( fontStyleClicked( int ) ) );
	gridLayout->addWidget( d-> m_font_style_list, 1, 1 );

	label = new QLabel( tr( "Size" ), this );
	gridLayout->addWidget( label, 2, 1 );

	d-> m_font_size_list = new QComboBox( this, "SizeListBox" );
	connect( d-> m_font_size_list, SIGNAL(  activated( int ) ),
			 this, SLOT( fontSizeClicked( int ) ) );
	gridLayout->addWidget( d-> m_font_size_list, 3, 1 );

	d-> m_pointbug = ( qt_version ( ) <= 233 );

	if ( withpreview ) {
		d-> m_preview = new QMultiLineEdit ( this, "Preview" );
		d-> m_preview-> setAlignment ( AlignCenter );
		d-> m_preview-> setWordWrap ( QMultiLineEdit::WidgetWidth );
		d-> m_preview-> setMargin ( 3 ); 
		d-> m_preview-> setText ( tr( "The Quick Brown Fox Jumps Over The Lazy Dog" ));
		gridLayout-> addRowSpacing ( 5, 4 );
		gridLayout-> addMultiCellWidget ( d-> m_preview, 6, 6, 0, 1 );
		gridLayout-> setRowStretch ( 6, 5 );
	}
	else
		d-> m_preview = 0;

	loadFonts ( d-> m_font_family_list );
}

OFontSelector::~OFontSelector ( )
{
	delete d;
}

bool OFontSelector::setSelectedFont ( const QFont &f )
{
	return setSelectedFont ( f. family ( ), d-> m_fdb. styleString ( f ), f. pointSize ( ), QFont::encodingName ( f. charSet ( )));
}

bool OFontSelector::setSelectedFont ( const QString &familyStr, const QString &styleStr, int sizeVal, const QString & /*charset*/ )
{
	QString sizeStr = QString::number ( sizeVal );

	QListBoxItem *family = d-> m_font_family_list-> findItem ( familyStr );
	if ( !family )
		family = d-> m_font_family_list-> findItem ( "Helvetica" );
	if ( !family )
		family = d-> m_font_family_list-> firstItem ( );
   	 d-> m_font_family_list-> setCurrentItem ( family );	
	fontFamilyClicked ( d-> m_font_family_list-> index ( family ));

	int style = findItemCB ( d-> m_font_style_list, styleStr );
	if ( style < 0 )
		style = findItemCB ( d-> m_font_style_list, "Regular" );
	if ( style < 0 && d-> m_font_style_list-> count ( ) > 0 )
		style = 0;
	d-> m_font_style_list-> setCurrentItem ( style );
	fontStyleClicked ( style );

	int size = findItemCB ( d-> m_font_size_list, sizeStr );
	if ( size < 0 )
		size = findItemCB ( d-> m_font_size_list, "10" );
	if ( size < 0 && d-> m_font_size_list-> count ( ) > 0 )
		size = 0;
	d-> m_font_size_list-> setCurrentItem ( size );
	fontSizeClicked ( size );
	
	return (( family ) && ( style >= 0 ) && ( size >= 0 ));
}

bool OFontSelector::selectedFont ( QString &family, QString &style, int &size )
{
	QString dummy;
	return selectedFont ( family, style, size, dummy );
}


QString OFontSelector::fontFamily ( ) const
{
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( d-> m_font_family_list-> currentItem ( ));
	
	return fli ? fli-> family ( ) : QString::null;
}

QString OFontSelector::fontStyle ( ) const
{
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( d-> m_font_family_list-> currentItem ( ));
	int fst = d-> m_font_style_list-> currentItem ( ); 

	return ( fli && fst >= 0 ) ? fli-> styles ( ) [fst] : QString::null;
}

int OFontSelector::fontSize ( ) const
{
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( d-> m_font_family_list-> currentItem ( ));
	int fsi = d-> m_font_size_list-> currentItem ( ); 

	return ( fli && fsi >= 0 ) ? fli-> sizes ( ) [fsi] : 10;
}

QString OFontSelector::fontCharSet ( ) const
{
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( d-> m_font_family_list-> currentItem ( ));

	return fli ? d-> m_fdb. charSets ( fli-> family ( )) [0] : QString::null;
}

bool OFontSelector::selectedFont ( QString &family, QString &style, int &size, QString &charset )
{
	int ffa = d-> m_font_family_list-> currentItem ( );
	int fst = d-> m_font_style_list-> currentItem ( ); 
	int fsi = d-> m_font_size_list-> currentItem ( );
	
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( ffa );
	
	if ( fli ) {
		family = fli-> family ( );
		style = fst >= 0 ? fli-> styles ( ) [fst] : QString::null;
		size = fsi >= 0 ? fli-> sizes ( ) [fsi] : 10;
		charset = d-> m_fdb. charSets ( fli-> family ( )) [0];

		return true;
	}
	else
		return false;
}


			

void OFontSelector::loadFonts ( QListBox *list )
{
	QStringList f = d-> m_fdb. families ( );
	
	for ( QStringList::ConstIterator it = f. begin ( ); it != f. end ( ); ++it ) {
		QValueList <int> ps = d-> m_fdb. pointSizes ( *it );
		
		if ( d-> m_pointbug ) {
			for ( QValueList <int>::Iterator it = ps. begin ( ); it != ps. end ( ); it++ )
				*it /= 10;
		}
	
		list-> insertItem ( new FontListItem ( *it, d-> m_fdb. styles ( *it ), ps ));
	}
}

void OFontSelector::fontFamilyClicked ( int index )
{
	QString oldstyle = d-> m_font_style_list-> currentText ( );
	QString oldsize  = d-> m_font_size_list-> currentText ( );
	
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( index );
	
	d-> m_font_style_list-> clear ( );	
	d-> m_font_style_list-> insertStringList ( fli-> styles ( ));
	d-> m_font_style_list-> setEnabled ( !fli-> styles ( ). isEmpty ( ));

	int i;
		
	i = findItemCB ( d-> m_font_style_list, oldstyle );
	if ( i < 0 )
		i = findItemCB ( d-> m_font_style_list, "Regular" );
	if (( i < 0 ) && ( d-> m_font_style_list-> count ( ) > 0 ))
		i = 0;
		
	if ( i >= 0 ) {
		d-> m_font_style_list-> setCurrentItem ( i );		
		fontStyleClicked ( i );
	}
	
	d-> m_font_size_list-> clear ( );
	QValueList<int> sl = fli-> sizes ( );
	
	for ( QValueList<int>::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) 
		d-> m_font_size_list-> insertItem ( QString::number ( *it ));

	i = findItemCB ( d-> m_font_size_list, oldsize );
	if ( i < 0 )
		i = findItemCB ( d-> m_font_size_list, "10" );
	if (( i < 0 ) && ( d-> m_font_size_list-> count ( ) > 0 ))
		i = 0;
		
	if ( i >= 0 ) {
		d-> m_font_size_list-> setCurrentItem ( i );									
		fontSizeClicked ( i );
	}
	changeFont ( );	
}

void OFontSelector::fontStyleClicked ( int /*index*/ )
{
	changeFont ( );	
}

void OFontSelector::fontSizeClicked ( int /*index*/ )
{
	changeFont ( );
}

void OFontSelector::changeFont ( )
{
	QFont f = selectedFont ( );

	if ( d-> m_preview )
		d-> m_preview-> setFont ( f );

	emit fontSelected ( f );
}


QFont OFontSelector::selectedFont ( )
{
	int ffa = d-> m_font_family_list-> currentItem ( );
	int fst = d-> m_font_style_list-> currentItem ( ); 
	int fsi = d-> m_font_size_list-> currentItem ( );
	
	FontListItem *fli = (FontListItem *) d-> m_font_family_list-> item ( ffa );
	
	if ( fli ) {
		return d-> m_fdb. font ( fli-> family ( ), \
		                         fst >= 0 ? fli-> styles ( ) [fst] : QString::null, \
		                         fsi >= 0 ? fli-> sizes ( ) [fsi] : 10, \
		                         d-> m_fdb. charSets ( fli-> family ( )) [0] );
	}
	else
		return QFont ( );
}


void OFontSelector::resizeEvent ( QResizeEvent *re )
{
	if ( d-> m_preview ) {
		d-> m_preview-> setMinimumHeight ( 1 );
		d-> m_preview-> setMaximumHeight ( 32767 );
	}

	QWidget::resizeEvent ( re );
	
	if ( d-> m_preview )
		d-> m_preview-> setFixedHeight ( d-> m_preview-> height ( ));
		
}
