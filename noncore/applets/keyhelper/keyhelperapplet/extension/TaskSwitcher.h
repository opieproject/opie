#ifndef _TASK_SWITCHER_H_
#define _TASK_SWITCHER_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qwindowsystem_qws.h>

#include <qpe/global.h>

#include "ExtensionInterface.h"

class TaskSwitcher : public ExtensionInterface
{
public:
	TaskSwitcher(const QString& kind = "switch");
	virtual ~TaskSwitcher();

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
private:
	int m_keycode;
	int m_keymask;
	QString m_kind;
	QValueList<int> m_modcodes;

	QStringList m_applist;
	QStringList::Iterator m_appit;

	void next();
};

#endif /* _TASK_SWITCHER_H_ */
