#ifndef _KEY_MODIFIERS_H_
#define _KEY_MODIFIERS_H_

#include <qmap.h>
#include <qtimer.h>
#include <qvaluelist.h>
#include <qnamespace.h>

#include <qpe/config.h>
#include "ConfigEx.h"

struct ModifierInfo {
public:
	ModifierInfo(const QString& tp, int code, int mask, bool mode)
	{
		type = tp;
		keycode = code;
		keymask = mask;
		pressed = false;
		toggled = false;
		toggle_mode = mode;
	}
	QString type;
	int keycode;
	int keymask;
	bool pressed;
	bool toggled;
	bool toggle_mode;
private:
} ;

class KeyModifiers : public QObject
{
	Q_OBJECT
public:
	typedef QValueList<ModifierInfo*> ModifierList;

	KeyModifiers();
	virtual ~KeyModifiers();
	void addType(const QString& type);
	ModifierInfo* assign(const QString& type, int keycode,
		int keymask = 0, bool toggle = false);
	void assignRelease(int keycode);
	void assignRelease(ModifierInfo* info, int keycode);
	void setToggle();
	void setToggle(ModifierInfo* info);

	bool isToggled();
	void resetStates();
	void keepToggles();

	bool pressKey(int keycode, int modifiers);
	void releaseKey(int keycode);
	int getState();
	int getState(int modifiers, bool reset = false);
	int getMask(const QString& type);
	int getModifiers(int modifiers);

	bool isModifier(int keycode);

	void statistics();

	void reset();
public slots:
	void resetToggles();
private:
	QMap<QString, int> m_types;
	int m_bitmask;
	ModifierList m_modifiers;
	QMap<int, ModifierList*> m_releasekeys;
	ModifierList m_togglekeys;
	ModifierInfo* m_info;
	QTimer* m_pTimer;
	int m_timeout;

	void clear();
	void init();
};

#endif /* _KEY_MODIFIERS_H_ */
