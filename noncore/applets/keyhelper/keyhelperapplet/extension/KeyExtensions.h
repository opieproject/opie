#ifndef _KEY_EXTENSIONS_H_
#define _KEY_EXTENSIONS_H_

#include <qstring.h>
#include <qstringlist.h>
#include "KeyNames.h"
#include "KeyModifiers.h"
#include "ExtensionFactory.h"

class KeyExtensions
{
public:
	KeyExtensions();
	~KeyExtensions();

	void setKeyModifiers(KeyModifiers* mod)
	{
		m_pModifiers = mod;
	}

	void assign(const QString& kind, int keycode,
		int keymask, const QValueList<int>& modcodes);
	void assign(const QString& kind, int keycode,
		int keymask, int modcode);
	bool doKey(int keycode, int keymask, bool isPress);

	void statistics();

	void reset();
	void init();
private:
	KeyModifiers* m_pModifiers;
	ExtensionFactory m_oExtFactory;
	int m_cancelcode;

	void clear();
};

#endif /* _KEY_EXTENSIONS_H_ */

