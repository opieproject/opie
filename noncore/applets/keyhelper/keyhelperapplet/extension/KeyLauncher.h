#ifndef _KEY_LAUNCHER_H_
#define _KEY_LAUNCHER_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/applnk.h>
#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>

#include "ExtensionInterface.h"
#include "KeyNames.h"
#include "LnkWrapper.h"
#include "ConfigEx.h"

class KeyLauncher : public ExtensionInterface
{
public:
	KeyLauncher(const QString& kind = "launch");
	virtual ~KeyLauncher();

	virtual bool onKeyPress(int keycode);
	virtual bool onModRelease(int /*modcode*/){return(false);}
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
};

#endif /* _KEY_LAUNCHER_H_ */

