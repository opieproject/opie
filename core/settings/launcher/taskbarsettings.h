#ifndef __TASKBAR_SETTINGS_H__
#define __TASKBAR_SETTINGS_H__

#include <qwidget.h>
#include <qmap.h>

class QListView;
class QCheckListItem;
class QCheckBox;


class TaskbarSettings : public QWidget {
	Q_OBJECT

public:
	TaskbarSettings ( QWidget *parent = 0, const char *name = 0 );

	void accept ( );

protected slots:
	void appletChanged ( );

protected:
	void init ( );

private:
	QListView *m_list;
	QCheckBox *m_omenu;
	QCheckBox *m_omenu_tabs;

	QMap <QString, QCheckListItem *> m_applets;
	bool m_applets_changed;
};

#endif
