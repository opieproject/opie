#ifndef __TABS_SETTINGS_H__
#define __TABS_SETTINGS_H__

#include <qwidget.h>
#include <qmap.h>

#include "tabconfig.h"

class QListBox;


class TabsSettings : public QWidget {
	Q_OBJECT

public:
	TabsSettings ( QWidget *parent = 0, const char *name = 0 );

	void accept ( );

protected slots:
	void newClicked ( );
	void deleteClicked ( );
	void editClicked ( );

protected:
	void init ( );
	void readTabSettings ( );

private:
	QListBox *m_list;
//	QString currentTab;
	QStringList m_ids;
	QMap <QString, TabConfig> m_tabs;
};




#endif
