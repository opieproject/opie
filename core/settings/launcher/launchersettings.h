#ifndef __LAUNCHER_SETTINGS_H__
#define __LAUNCHER_SETTINGS_H__

#include <qdialog.h>

class TabsSettings;
class TaskbarSettings;

class LauncherSettings : public QDialog {
	Q_OBJECT

public:
	LauncherSettings ( );

	virtual void accept ( );
	virtual void done ( int r );

private:
	TabsSettings *m_tabs;
	TaskbarSettings *m_taskbar;
};

#endif
