#include "KeyLauncher.h"
#include "KHUtil.h"

KeyLauncher::KeyLauncher(const QString& kind) : m_kind(kind)
{
	qDebug("KeyLauncher::KeyLauncher()");
}

KeyLauncher::~KeyLauncher()
{
	qDebug("KeyLauncher::~KeyLauncher()");
}

bool KeyLauncher::onKeyPress(int keycode)
{
	QString key;
	QStringList args;
	ConfigEx& cfg = ConfigEx::getInstance("keyhelper");

	key = KeyNames::getName(keycode);
	if(key == QString::null){
		return(false);
	}

	QString group = kind();
	group[0] = group[0].upper();

	/* read application launcher */
	QString app = KHUtil::currentApp();
	if(!app.isEmpty()){
		cfg.setGroup(group + "_" + app);
		/* read config */
		args = cfg.readListEntry(key, '\t');
	}

	/* read default launcher */
	if(args.isEmpty()){
		cfg.setGroup(group);

		/* read config */
		args = cfg.readListEntry(key, '\t');
	}

	if(args.isEmpty()){
		return(false);
	}

	/* launch application */
	LnkWrapper lnk(args);
	if(lnk.isValid()){
		//args.remove(args.begin());
		lnk.instance().execute();
	}

	return(true);
}

