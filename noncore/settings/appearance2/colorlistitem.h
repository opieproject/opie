#ifndef COLORLISTITEM_H
#define COLORLISTITEM_H

#include <qlistbox.h>
#include <qpalette.h>
#include <qapplication.h>

#include <qpe/config.h>

class Appearance;

class ColorListItem : public QListBoxText {
public:
	ColorListItem ( const QString &t, Config &cfg ) : QListBoxText ( t ) 
	{
		m_colors = new QColor [s_colorcount];
		load ( cfg );
	}

	virtual ~ColorListItem ( )
	{
		delete [] m_colors;
	}
	
	QPalette palette ( )
	{
		return m_palette;
	}

	bool load ( Config &cfg )
	{
		for ( int i = 0; i < s_colorcount; i++ ) 
			m_colors [i] = QColor ( cfg. readEntry ( s_colorlut [i]. m_key, s_colorlut [i]. m_def ));
			
		buildPalette ( );
		return true;
	}
	
	void buildPalette ( )
	{
		m_palette = QPalette ( m_colors [r2i(QColorGroup::Button)], m_colors [r2i(QColorGroup::Background)] );
		m_palette. setColor ( QColorGroup::Highlight, m_colors [r2i(QColorGroup::Highlight)] );
		m_palette. setColor ( QColorGroup::HighlightedText, m_colors [r2i(QColorGroup::HighlightedText)] );
		m_palette. setColor ( QColorGroup::Text, m_colors [r2i(QColorGroup::Text)] );
		m_palette. setColor ( QPalette::Active, QColorGroup::ButtonText, m_colors [r2i(QColorGroup::ButtonText)] );
		m_palette. setColor ( QColorGroup::Base, m_colors [r2i(QColorGroup::Base)] );
		m_palette. setColor ( QPalette::Disabled, QColorGroup::Text, m_palette. color ( QPalette::Active, QColorGroup::Background ). dark ( ));
	}
	
	bool save ( Config &cfg )
	{
		for ( int i = 0; i < s_colorcount; i++ ) 
			cfg. writeEntry ( s_colorlut [i]. m_key, m_colors [i]. name ( ));
		return true;
	}

	QColor color ( QColorGroup::ColorRole role )
	{
		int i = r2i ( role );
		return i >= 0 ? m_colors [i] : QColor ( );
	}
	
	void setColor ( QColorGroup::ColorRole role, QColor c )
	{
		int i = r2i ( role );
		if ( i >= 0 ) {
			m_colors [i] = c;
			buildPalette ( );
		}
	}
	
	QString label ( QColorGroup::ColorRole role )
	{
		int i = r2i ( role );
		return i >= 0 ? qApp-> translate ( "Appearance", s_colorlut [i]. m_label ) : QString::null;
	}
		
private:
	QPalette m_palette;	
	QColor *m_colors;
	
	static struct colorlut {
		QColorGroup::ColorRole m_role;
		const char *           m_key;
		const char *           m_def;		
		const char *           m_label;
	} const s_colorlut [];
	static const int s_colorcount;

	static int r2i ( QColorGroup::ColorRole role )
	{
		for ( int i = 0; i < s_colorcount; i++ ) {
			if ( s_colorlut [i]. m_role == role )
				return i;
		}
		return -1;
	}
};

const ColorListItem::colorlut ColorListItem::s_colorlut [] = {
	{ QColorGroup::Base,            "Base",            "#FFFFFF", QT_TRANSLATE_NOOP( "Appearance", "Base" )             },
	{ QColorGroup::Background,      "Background",      "#E5E1D5", QT_TRANSLATE_NOOP( "Appearance", "Background" )       },
	{ QColorGroup::Button,          "Button",          "#D6CDBB", QT_TRANSLATE_NOOP( "Appearance", "Button" )           },
	{ QColorGroup::ButtonText,      "ButtonText",      "#000000", QT_TRANSLATE_NOOP( "Appearance", "Button Text" )      }, 
	{ QColorGroup::Highlight,       "Highlight",       "#800000", QT_TRANSLATE_NOOP( "Appearance", "Highlight" )        },
	{ QColorGroup::HighlightedText, "HighlightedText", "#FFFFFF", QT_TRANSLATE_NOOP( "Appearance", "Highlighted Text" ) },
	{ QColorGroup::Text,            "Text",            "#000000", QT_TRANSLATE_NOOP( "Appearance", "Text" )             }
};

const int ColorListItem::s_colorcount =  sizeof( s_colorlut ) / sizeof ( s_colorlut [0] );


#endif
