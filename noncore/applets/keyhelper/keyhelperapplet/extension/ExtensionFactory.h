#ifndef _EXTENSION_FACTORY_H_
#define _EXTENSION_FACTORY_H_

#include "ExtensionInterface.h"
#include "TaskSwitcher.h"
#include "KeyLauncher.h"
#include "TaskSelector.h"
#include "MenuLauncher.h"

class ExtensionFactory
{
public:
	ExtensionFactory();
	virtual ~ExtensionFactory();

	ExtensionInterface* createInstance(const QString& kind);
	ExtensionInterface* createInstance(const QString& kind,
		int keycode, int keymask);
	ExtensionList& getList()
	{
		return(m_oExtList);
	}
	void clear();
	void reset();
	void sweep();
private:
	ExtensionList m_oExtList;
	ExtensionList* m_pLoadList;

	ExtensionInterface* loadInstance(const QString& kind,
		int keycode, int keymask);
};

#endif /* _EXTENSION_FACTORY_H_ */

