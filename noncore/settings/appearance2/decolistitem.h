#ifndef DECOLISTITEM_H
#define DECOLISTITEM_H

#include <qpe/windowdecorationinterface.h>
#include <qlistbox.h>

class DecoListItem : public QListBoxText {
public:
	DecoListItem ( const QString &t ) : QListBoxText ( t ) 
	{ 
		m_lib = 0;
		m_window_if = 0;
//		m_settings_if = 0;
	}

	DecoListItem ( QLibrary *lib, WindowDecorationInterface *iface ) : QListBoxText ( iface-> name ( ))
	{
		m_lib = lib;
		m_window_if = iface;
		
//		iface-> queryInterface ( IID_WindowDecoration, (QUnknownInterface **) &m_settings_if );		
	}
	
	virtual ~DecoListItem ( )
	{
//		if ( m_settings_if )
//			m_settings_if-> release ( );
		if ( m_window_if )
			m_window_if-> release ( );
		delete m_lib;
	}
	
	bool hasSettings ( ) const
	{
//		return ( m_settings_if );
		return false;
	}

	QWidget *settings ( QWidget * /*parent*/ )
	{
//		return m_settings_if ? m_settings_if-> create ( parent ) : 0;
		return 0;
	}
	
	bool setSettings ( bool /*accepted*/ ) 
	{
//		if ( !m_settings_if )
//			return false;
	
//		if ( accepted )
//			return m_settings_if-> accept ( );
//		else {
//			m_settings_if-> reject ( );
//			return false;
//		}
		return false;
	}

	QString key ( )
	{
		if ( m_window_if )
			return QString ( m_window_if-> name ( ));
		else
			return text ( );
	}
	
	WindowDecorationInterface *interface ( )
	{
		return m_window_if;
	}

private:
	QLibrary *m_lib;
	WindowDecorationInterface *m_window_if;
//	WindowDecorationSettingsInterface *m_settings_if;

};

#endif