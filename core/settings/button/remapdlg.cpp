#include <qlistview.h>
#include <qlabel.h>
#include <qcombobox.h>

#include "remapdlg.h"
#include "buttonutils.h"

using namespace Opie;

class NoSortItem : public QListViewItem {
public:
	NoSortItem ( QListView *lv, uint pos, const QString &str, const QCString &s1 = 0, const QCString &s2 = 0 )
		: QListViewItem ( lv, str, s1, s2 )
	{
		m_key = QString ( QChar ( 'a' + pos ));
		m_def = false;
	}
	
	void setDefault ( bool b )
	{
		m_def = b;
	}
	
	virtual QString key ( int /*column*/, bool /*ascending*/ ) const
	{
		return m_key;
	}
	
	virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align ) 
	{
		if ( m_def ) {
			QFont f ( listView ( )-> font ( ));			
			f. setBold ( true );		
			p-> setFont ( f );
		}
		QListViewItem::paintCell ( p, cg, column, width, align );	
	}
	
private:
	QString m_key;
	bool m_def;
};


RemapDlg::RemapDlg ( const Opie::ODeviceButton *b, bool hold, QWidget *parent, const char *name )
	: RemapDlgBase ( parent, name, true, WStyle_ContextHelp )
{
	setCaption ( tr( "%1 %2", "(hold|press) buttoname" ). arg( hold ? tr( "Held" ) : tr( "Pressed" )). arg ( b-> userText ( )));
	
	m_msg = hold ? b-> heldAction ( ) : b-> pressedAction ( );
	m_msg_preset = hold ? b-> factoryPresetHeldAction ( ) : b-> factoryPresetPressedAction ( );
	
	m_map_none   = new NoSortItem ( w_list, 0, tr( "No mapping" ));
	m_map_preset = new NoSortItem ( w_list, 1, tr( "Default" ), m_msg_preset. channel ( ), m_msg_preset. message ( ));
	((NoSortItem *) m_map_preset )-> setDefault ( true );
	m_map_custom = new NoSortItem ( w_list, 2, tr( "Custom" ), m_msg. channel ( ), m_msg. message ( ));
	
	QListViewItem *it = new NoSortItem ( w_list, 3, tr( "Actions" ));	
	ButtonUtils::inst ( )-> insertActions ( it );
	it-> setOpen ( true );
	
	it = new NoSortItem ( w_list, 4, tr( "Show" ));	
	ButtonUtils::inst ( )-> insertAppLnks ( it );
	
	m_current = m_map_custom;
	w_list-> setCurrentItem ( m_current );
	
	static const char * const def_channels [] = {
		"QPE/Application/", "QPE/Launcher", "QPE/System", "QPE/TaskBar", "QPE/", 0
	};
	
	w_channel-> insertStrList ((const char **) def_channels );
}

RemapDlg::~RemapDlg ( )
{
}

void RemapDlg::itemChanged ( QListViewItem *it )
{
	bool enabled = false;
	OQCopMessage m;
	
	m_current = it;

	if ( it == m_map_none )
		m_msg = m = OQCopMessage ( 0, 0 );
	else if ( it == m_map_preset )
		m_msg = m = m_msg_preset;
	else if ( it && !it-> childCount ( )) {
		enabled = ( it == m_map_custom );
		m_msg = m = OQCopMessage ( it-> text ( 1 ). latin1 ( ), it-> text ( 2 ). latin1 ( ));
	}
	
	w_channel-> setEnabled ( enabled );
	w_message-> setEnabled ( enabled );
	
	w_channel-> setEditText ( m. channel ( ));
	w_message-> setEditText ( m. message ( ));
}

void RemapDlg::textChanged ( const QString &str )
{
	if ( !m_current )
		return;

	QComboBox *which = (QComboBox *) sender ( );

	if ( which == w_channel )
		m_current-> setText ( 1, str );
	else if ( which == w_message )
		m_current-> setText ( 2, str );
}

OQCopMessage RemapDlg::message ( )
{
	return m_msg;
}

