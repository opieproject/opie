#ifndef MINIKDE_KEMAILSETTINGS_H
#define MINIKDE_KEMAILSETTINGS_H

#include <qstring.h>

class KEMailSettings
{
  public:
	enum Setting {
		ClientProgram,
		ClientTerminal,
		RealName,
		EmailAddress,
		ReplyToAddress,
		Organization,
		OutServer,
		OutServerLogin,
		OutServerPass,
		OutServerType,
		OutServerCommand,
		OutServerTLS,
		InServer,
		InServerLogin,
		InServerPass,
		InServerType,
		InServerMBXType,
		InServerTLS
	};
	QString getSetting(KEMailSettings::Setting s);
};

#endif
