#ifndef STYLELISTITEM_H
#define STYLELISTITEM_H

#include <qlistbox.h>
#include <qpe/styleinterface.h>

#include <stdio.h>

class StyleListItem : public QListBoxText {
public:
	StyleListItem ( const QString &t, QStyle *sty ) : QListBoxText ( t ) 
	{ 
		m_lib = 0;
		m_style_if = 0;
		m_settings_if = 0;
		m_style = sty;
	}

	StyleListItem ( QLibrary *lib, StyleInterface *iface ) : QListBoxText ( iface-> name ( ))
	{
		m_lib = lib;
		m_style_if = iface;
		m_settings_if = 0;
		m_style = iface-> create ( );
		
		iface-> queryInterface ( IID_StyleSettings, (QUnknownInterface **) &m_settings_if );		
	}
	
	virtual ~StyleListItem ( )
	{
		delete m_style;
	
		if ( m_settings_if )
			m_settings_if-> release ( );
		if ( m_style_if )
			m_style_if-> release ( );
		delete m_lib;
	}
	
	bool hasSettings ( ) const
	{
		return ( m_settings_if );
	}

	QWidget *settings ( QWidget *parent )
	{
		return m_settings_if ? m_settings_if-> create ( parent ) : 0;
	}
	
	bool setSettings ( bool accepted ) 
	{
		if ( !m_settings_if )
			return false;
	
		if ( accepted )
			return m_settings_if-> accept ( );
		else {
			m_settings_if-> reject ( );
			return false;
		}
	}

	QString key ( )
	{
		if ( m_style_if )
			return QString ( m_style_if-> key ( ));
		else
			return text ( );
	}
	
	QStyle *style ( ) 
	{
		return m_style;
	}

private:
	QLibrary *m_lib;
	QStyle *m_style;
	StyleInterface *m_style_if;
	StyleSettingsInterface *m_settings_if;

};


#endif
