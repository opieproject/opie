#ifndef _MENU_LAUNCHER_H_
#define _MENU_LAUNCHER_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qwindowsystem_qws.h>
#define INCLUDE_MENUITEM_DEF
#include <qpopupmenu.h>
#include <qpoint.h>
#include <qtimer.h>
#include <qgfx_qws.h>

#include <qpe/global.h>
#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include "ExtensionInterface.h"
#include "MenuTitle.h"
#include "KeyNames.h"
#include "ConfigEx.h"
#include "LnkWrapper.h"
#include "QPopupMenuEx.h"

struct ItemInfo{
	ItemInfo(QString g=QString::null, QString e=QString::null)
		: group(g), entry(e){}
	QString group;
	QString entry;
};

class MenuLauncher : public QObject, public ExtensionInterface
{
	Q_OBJECT
public:
	MenuLauncher(const QString& kind = "menu");
	virtual ~MenuLauncher();

	typedef QValueList<ItemInfo> ItemList;

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
	void select(int id);
	void highlight(int id);
private:
	int m_keycode;
	int m_keymask;
	QString m_kind;
	QValueList<int> m_modcodes;

	int m_submenuTimeout;
	bool m_isShowing;
	bool m_enablePopup;
	int m_id;
	QPopupMenu* m_pMenu;
	QPopupMenu* m_pTopMenu;
	QDateTime m_lastmodify;

	QMap<QObject*, int> m_oLastId;
	QValueList<QPopupMenu*> m_oMenuList;
	ItemList m_oItemList;
	QTimer* m_pTimer;

	QStringList m_args;

	QString getMenuText(const QString& key, const QString& name);
	QPopupMenu* initMenu(QWidget* parent, const QString& name);
	int buildMenu(bool force=false);
	int buildMenu(const QString& section, QPopupMenu* pMenu, int& id);
	void clearSubMenu();
	void init();
	void next();
private slots:
	bool eventFilter(QObject* o, QEvent* e);
	void execute();
};

#endif /* _MENU_LAUNCHER_H_ */
