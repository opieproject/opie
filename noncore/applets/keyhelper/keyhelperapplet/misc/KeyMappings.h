#ifndef _KEY_MAPPINGS_H_
#define _KEY_MAPPINGS_H_

#include <qwindowsystem_qws.h>
#include <qmap.h>

#include <qintdict.h>

class MapInfo
{
public:
	MapInfo(){};
	MapInfo(int code, int mod = 0, int uni = 0,
		int shift_uni = 0, int ctrl_uni = 0);

	static const QWSServer::KeyMap* findKeyMap(int code);

	int keycode;
	int modifiers;
	int unicode;
	int shift_unicode;
	int ctrl_unicode;
	bool isDefined;
};

class KeyMappings
{
public:
	typedef QWSServer::KeyMap KeyMap;
	typedef QMap<int, MapInfo> CodeMaps;

	KeyMappings();
	virtual ~KeyMappings();

	void setOriginal(int unicode, int modifiers);
	void setModifiers(int modifiers);
	void setUnicode(int unicode);
	void setKeycode(int keycode);

	int getUnicode();
	int getKeycode();
	int getModifiers();
	bool isDefined();

	void assign(int keycode, int keymask, int mapcode, int mapmodifiers = 0,
		int unicode = 0, int shift_unicode = 0, int ctrl_unicode = 0);
	void assignModifier(const QString& type, const QString& state);
	void assignUnicode(const QString& kind, const QString& ch);
	void assignUnicode(int unicode);
	bool apply(int unicode, int keycode, int modifiers, int keymask, bool isPress);
	bool apply(int keycode, int keymask, bool isPress);
	void setCapsLock(bool on=true){
		m_capslock = on;
	}

	void reset();

	void statistics();
private:
	QMap<int, CodeMaps*> m_keymaps;
	MapInfo m_keyinfo;
	int m_modifiers;
	CodeMaps::Iterator m_it;

	bool m_capslock;
	bool m_isMapped;

	void init();
	void clear();
};

#endif /* _KEY_MAPPINGS_H_ */
