#ifndef _KEY_ACTION_H_
#define _KEY_ACTION_H_

#include <qobject.h>
#include <qwindowsystem_qws.h>
#include <qtimer.h>
#include <qpe/power.h>
#include "KeyMappings.h"
#include "KeyModifiers.h"
#include "KeyExtensions.h"
#include "KeyRepeater.h"
#include "ConfigEx.h"

class KeyAction : public QObject
{
	Q_OBJECT
public:
	KeyAction();
	virtual ~KeyAction();

	void setKeyMappings(KeyMappings* map)
	{
		m_pMappings = map;
	}
	void setKeyModifiers(KeyModifiers* mod)
	{
		m_pModifiers = mod;
	}
	void setKeyExtensions(KeyExtensions* ext)
	{
		m_pExtensions = ext;
	}
	void setKeyRepeater(KeyRepeater* rep)
	{
		m_pRepeater = rep;
		connect(m_pRepeater, SIGNAL(keyEvent(int,int,int,bool,bool)),
			this, SLOT(sendKeyEvent(int,int,int,bool,bool)));
	}
	void setAction(int unicode, int keycode, int modifiers,
		bool isPress, bool autoRepeat);
	bool doAction();

	void enable(){
		init();
		m_enable = true;
	};
	void disable(){
		m_enable = false;
	};
	void setCapture(bool enable){
		m_capture = enable;
	}

	void setHook(const QCString& s){
		m_hookChannel = s;
	}
private:
	int m_unicode;
	int m_keycode;
	int m_modifiers;
	bool m_isPress;
	bool m_autoRepeat;

	bool m_capture;
	bool m_enable;
	int m_presscnt;

	int m_keep_toggle_code;
	bool m_keepToggle;
	bool m_check;
	QValueList<int> m_excludeKeys;

	QCString m_hookChannel;

	KeyMappings* m_pMappings;
	KeyModifiers* m_pModifiers;
	KeyExtensions* m_pExtensions;
	KeyRepeater* m_pRepeater;

	void init();
	bool checkState();
private slots:
	void sendKeyEvent(int unicode, int keycode, int modifiers,
		bool isPress, bool autoRepeat);
};

#endif /* _KEY_ACTION_H_ */
