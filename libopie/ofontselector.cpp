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

#include "ofontselector.h"


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



OFontSelector::OFontSelector ( QWidget *parent, const char *name, WFlags fl ) : QWidget ( parent, name, fl )
{
    QGridLayout *gridLayout = new QGridLayout ( this, 0, 0, 4, 4 );
    gridLayout->setRowStretch ( 4, 10 );

    m_font_family_list = new QListBox( this, "FontListBox" );
    gridLayout->addMultiCellWidget( m_font_family_list, 0, 4, 0, 0 );
    connect( m_font_family_list, SIGNAL( highlighted( int ) ), this, SLOT( fontFamilyClicked( int ) ) );

    QLabel *label = new QLabel( tr( "Style" ), this );
    gridLayout->addWidget( label, 0, 1 );

    m_font_style_list = new QComboBox( this, "StyleListBox" );
    connect( m_font_style_list, SIGNAL(  activated( int ) ), this, SLOT( fontStyleClicked( int ) ) );
    gridLayout->addWidget( m_font_style_list, 1, 1 );

    label = new QLabel( tr( "Size" ), this );
    gridLayout->addWidget( label, 2, 1 );

    m_font_size_list = new QComboBox( this, "SizeListBox" );
    connect( m_font_size_list, SIGNAL(  activated( int ) ),
             this, SLOT( fontSizeClicked( int ) ) );
    gridLayout->addWidget( m_font_size_list, 3, 1 );

	loadFonts ( m_font_family_list );
}

OFontSelector::~OFontSelector ( )
{
}

bool OFontSelector::setSelectedFont ( const QFont &f )
{
	return setSelectedFont ( f. family ( ), m_fdb. styleString ( f ), f. pointSize ( ), QFont::encodingName ( f. charSet ( )));
}

bool OFontSelector::setSelectedFont ( const QString &familyStr, const QString &styleStr, int sizeVal, const QString & /*charset*/ )
{
	QString sizeStr = QString::number ( sizeVal );

	QListBoxItem *family = m_font_family_list-> findItem ( familyStr );
	if ( !family )
		family = m_font_family_list-> findItem ( "Helvetica" );
	if ( !family )
		family = m_font_family_list-> firstItem ( );
   	 m_font_family_list-> setCurrentItem ( family );    
    fontFamilyClicked ( m_font_family_list-> index ( family ));

	int style = findItemCB ( m_font_style_list, styleStr );
	if ( style < 0 )
		style = findItemCB ( m_font_style_list, "Regular" );
	if ( style < 0 && m_font_style_list-> count ( ) > 0 )
		style = 0;
	m_font_style_list-> setCurrentItem ( style );
	fontStyleClicked ( style );

	int size = findItemCB ( m_font_size_list, sizeStr );
	if ( size < 0 )
		size = findItemCB ( m_font_size_list, "10" );
	if ( size < 0 && m_font_size_list-> count ( ) > 0 )
		size = 0;
	m_font_size_list-> setCurrentItem ( size );
	fontSizeClicked ( size );
	
	return (( family ) &&
	        ( style >= 0 ) &&
	        ( size >= 0 ));
}

bool OFontSelector::selectedFont ( QString &family, QString &style, int &size )
{
	QString dummy;
	return selectedFont ( family, style, size, dummy );
}


QString OFontSelector::fontFamily ( ) const
{
	FontListItem *fli = (FontListItem *) m_font_family_list-> item ( m_font_family_list-> currentItem ( ));
	
	return fli ? fli-> family ( ) : QString::null;
}

QString OFontSelector::fontStyle ( ) const
{
	FontListItem *fli = (FontListItem *) m_font_family_list-> item ( m_font_family_list-> currentItem ( ));
    int fst = m_font_style_list-> currentItem ( ); 

	return ( fli && fst >= 0 ) ? fli-> styles ( ) [fst] : QString::null;
}

int OFontSelector::fontSize ( ) const
{
	FontListItem *fli = (FontListItem *) m_font_family_list-> item ( m_font_family_list-> currentItem ( ));
    int fsi = m_font_size_list-> currentItem ( ); 

	return ( fli && fsi >= 0 ) ? fli-> sizes ( ) [fsi] : 10;
}

QString OFontSelector::fontCharSet ( ) const
{
	FontListItem *fli = (FontListItem *) m_font_family_list-> item ( m_font_family_list-> currentItem ( ));

	return fli ? m_fdb. charSets ( fli-> family ( )) [0] : QString::null;
}

bool OFontSelector::selectedFont ( QString &family, QString &style, int &size, QString &charset )
{
    int ffa = m_font_family_list-> currentItem ( );
    int fst = m_font_style_list-> currentItem ( ); 
    int fsi = m_font_size_list-> currentItem ( );
    
    FontListItem *fli = (FontListItem *) m_font_family_list-> item ( ffa );
    
    if ( fli ) {
    	family = fli-> family ( );
    	style = fst >= 0 ? fli-> styles ( ) [fst] : QString::null;
    	size = fsi >= 0 ? fli-> sizes ( ) [fsi] : 10;
    	charset = m_fdb. charSets ( fli-> family ( )) [0];

		return true;
	}
	else
		return false;
}


            

void OFontSelector::loadFonts ( QListBox *list )
{
	QStringList f = m_fdb. families ( );
	
	for ( QStringList::ConstIterator it = f. begin ( ); it != f. end ( ); ++it )
		list-> insertItem ( new FontListItem ( *it, m_fdb. styles ( *it ), m_fdb. pointSizes ( *it )));
}

void OFontSelector::fontFamilyClicked ( int index )
{
	QString oldstyle = m_font_style_list-> currentText ( );
	QString oldsize  = m_font_size_list-> currentText ( );
	
    FontListItem *fli = (FontListItem *) m_font_family_list-> item ( index );
    
	m_font_style_list-> clear ( );    
	m_font_style_list-> insertStringList ( fli-> styles ( ));
	m_font_style_list-> setEnabled ( !fli-> styles ( ). isEmpty ( ));

	int i;
		
	i = findItemCB ( m_font_style_list, oldstyle );
	if ( i < 0 )
		i = findItemCB ( m_font_style_list, "Regular" );
	if (( i < 0 ) && ( m_font_style_list-> count ( ) > 0 ))
		i = 0;
		
	if ( i >= 0 ) {
		m_font_style_list-> setCurrentItem ( i );		
		fontStyleClicked ( i );
	}
	
	m_font_size_list-> clear ( );
	QValueList<int> sl = fli-> sizes ( );
	
	for ( QValueList<int>::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) 
		m_font_size_list-> insertItem ( QString::number ( *it ));

	i = findItemCB ( m_font_size_list, oldsize );
	if ( i < 0 )
		i = findItemCB ( m_font_size_list, "10" );
	if (( i < 0 ) && ( m_font_size_list-> count ( ) > 0 ))
		i = 0;
		
	if ( i >= 0 ) {
		m_font_size_list-> setCurrentItem ( i );	                                
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
	emit fontSelected ( selectedFont ( ));
}


QFont OFontSelector::selectedFont ( )
{
    int ffa = m_font_family_list-> currentItem ( );
    int fst = m_font_style_list-> currentItem ( ); 
    int fsi = m_font_size_list-> currentItem ( );
    
    FontListItem *fli = (FontListItem *) m_font_family_list-> item ( ffa );
    
    if ( fli ) {
		return m_fdb. font ( fli-> family ( ), \
		                     fst >= 0 ? fli-> styles ( ) [fst] : QString::null, \
		                     fsi >= 0 ? fli-> sizes ( ) [fsi] : 10, \
		                     m_fdb. charSets ( fli-> family ( )) [0] );
	}
	else
		return QFont ( );
}


                