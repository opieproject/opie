#ifndef _TASK_SELECTOR_H_
#define _TASK_SELECTOR_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qwindowsystem_qws.h>
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qtimer.h>
#include <qgfx_qws.h>

#include <qpe/global.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/mimetype.h>

#include "ExtensionInterface.h"
#include "MenuTitle.h"
#include "KeyNames.h"
#include "AppLnkManager.h"
#include "ConfigEx.h"
#include "QPopupMenuEx.h"

class TaskSelector : public QObject, public ExtensionInterface
{
	Q_OBJECT
public:
	TaskSelector(const QString& kind = "select");
	virtual ~TaskSelector();

	virtual bool onKeyPress(int keycode);
	virtual bool onModRelease(int modcode);
	virtual int getKeycode()
	{
		return(m_keycode);
	}
	virtual int getKeymask()
	{
		return(m_keymask);
	}
	virtual const QValueList<int>& getModcodes()
	{
		return(m_modcodes);
	}
	virtual void setKeycode(int keycode)
	{
		m_keycode = keycode;
	}
	virtual void setKeymask(int keymask)
	{
		m_keymask = keymask;
	}
	virtual void setModcodes(const QValueList<int>& modcodes)
	{
		m_modcodes = modcodes;
	}
	virtual const QString& kind()
	{
		return(m_kind);
	}
public slots:
	void show();
	void select();
	void select(int);
	void highlight(int id);
private:
	int m_keycode;
	int m_keymask;
	QString m_kind;
	QValueList<int> m_modcodes;

	bool m_isShowing;
	int m_index;
	QPopupMenu* m_pMenu;
	QStringList m_applist;
	QString m_accesskeys;

	int buildMenu();
	void next();
private slots:
	bool eventFilter(QObject* o, QEvent* e);
};

#endif /* _TASK_SELECTOR_H_ */
