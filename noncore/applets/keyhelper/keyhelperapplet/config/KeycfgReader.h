#ifndef _KEYCFG_READER_H_
#define _KEYCFG_READER_H_

#include <stdlib.h>
#include <qxml.h>
#include <qstring.h>
#include <qdir.h>
#include "KeycfgHandler.h"
#include "KeycfgErrorHandler.h"
#include "KeyNames.h"
#include "ConfigEx.h"

class KeycfgReader
{
public:
	KeycfgReader();
	~KeycfgReader();

	bool load();
	bool load(const QString& path);

	inline void setKeyModifiers(KeyModifiers* mod)
	{
		m_pModifiers = mod;
	}
	inline void setKeyMappings(KeyMappings* map)
	{
		m_pMappings = map;
	}
	inline void setKeyExtensions(KeyExtensions* ext)
	{
		m_pExtensions = ext;
	}
	inline void setKeyRepeater(KeyRepeater* rep)
	{
		m_pRepeater = rep;
	}

private:
	KeyModifiers* m_pModifiers;
	KeyMappings* m_pMappings;
	KeyExtensions* m_pExtensions;
	KeyRepeater* m_pRepeater;
};

#endif /* _KEYCFG_READER_H_ */
