#ifndef __OPIE_THEME_SET_H__
#define __OPIE_THEME_SET_H__

#include <qdialog.h>

class QListView;

class ThemeSet : public QDialog {
	Q_OBJECT
	
public:
	ThemeSet ( QWidget *parent = 0, const char *name = 0, WFlags fl = 0 );

protected:
	virtual void accept ( );
	
private:
	QListView *m_list;
};
#endif
