
#include <qlayout.h>

#include <opie/otabwidget.h>

#include "launchersettings.h"
#include "tabssettings.h"
#include "taskbarsettings.h"


LauncherSettings::LauncherSettings ( ) : QDialog ( 0, "LauncherSettings", false )
{
	setCaption ( tr( "Launcher Settings" ));

	QVBoxLayout *lay = new QVBoxLayout ( this, 4, 4 );

	OTabWidget *tw = new OTabWidget ( this, "otab" );
	lay-> addWidget ( tw );

	m_tabs = new TabsSettings ( tw );
	m_taskbar = new TaskbarSettings ( tw );

	tw-> addTab ( m_taskbar, "appearance/styletabicon.png", tr( "Taskbar" ));
	tw-> addTab ( m_tabs, "appearance/styletabicon.png", tr( "Tabs" ));
}

void LauncherSettings::accept ( )
{
	m_taskbar-> accept ( );
	m_tabs-> accept ( );

	QDialog::accept ( );
}

void LauncherSettings::done ( int r )
{
	QDialog::done ( r );
	close ( );
}
