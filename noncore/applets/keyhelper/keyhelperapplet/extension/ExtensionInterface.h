#ifndef _EXTENSION_INTERFACE_H_
#define _EXTENSION_INTERFACE_H_

#include <qvaluelist.h>

class ExtensionInterface
{
public:
	//ExtensionInterface();
	virtual ~ExtensionInterface(){}
	virtual bool onKeyPress(int keycode) = 0;
	virtual bool onModRelease(int modcode) = 0;
	virtual int getKeycode() = 0;
	virtual int getKeymask() = 0;
	virtual const QValueList<int>& getModcodes() = 0;
	virtual void setKeycode(int keycode) = 0;
	virtual void setKeymask(int keymask) = 0;
	virtual void setModcodes(const QValueList<int>& modcodes) = 0;
	virtual const QString& kind() = 0;
private:
};

typedef QValueList<ExtensionInterface*> ExtensionList;

#endif /* _EXTENSION_INTERFACE_H_ */

