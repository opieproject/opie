#ifndef __OPIE_THEME_SETTINGS_H__
#define __OPIE_THEME_SETTINGS_H__

#include <qdialog.h>

class QListView;

class ThemeSettings : public QWidget {
	Q_OBJECT
	
public:
	ThemeSettings ( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );

public:
	virtual bool writeConfig ( );
	
private:
	QListView *m_list;
};
#endif
