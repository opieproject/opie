#ifndef _CONFIG_EX_H_
#define _CONFIG_EX_H_

#include <qstringlist.h>
#include <qdatetime.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#define QTOPIA_INTERNAL_LANGLIST
#include <qpe/config.h>
#include <qpe/global.h>

#define CONFIG_MULTICODEC

/* Singleton Object */
class ConfigEx : public Config
{
public:
	static ConfigEx& getInstance(const QString& name){
		static ConfigEx cfg(name);
		if(/*cfg.flush() ||*/cfg.m_lastRead < cfg.lastModified()){
			cfg.load(&cfg);
		}
		return(cfg);
	}

#if 0
	bool flush(){
		if(changed){
			write();
			return(true);
		} else {
			return(false);
		}
	}
#endif

	const QString& getGroup(){
		return(git.key());
	}

	void load(ConfigEx* cfg){
		cfg->read();
		cfg->decode();
		cfg->m_lastRead = QDateTime::currentDateTime();
	}

	void setConfig(const QString& name){
		if(name == QString::null){
			return;
		}
		/*flush();*/
		filename = configFilename(name, User);
		load(this);
	}

	void reload() {
		/*flush();*/
		load(this);
	}

	QStringList getKeys();
	QDateTime lastModified();
	QDateTime lastRead(){
		return(m_lastRead);
	}

	friend class Dummy;	 /* for compie warning */
private:
	ConfigEx(const QString& name, Domain domain=User);
	ConfigEx& operator=(const ConfigEx&);
	virtual ~ConfigEx(){changed = false;}

	class Dummy{}; /* for compile warning */


	void read();
	void decode();
	//void removeComment();

	QDateTime m_lastRead;
#ifdef CONFIG_MULTICODEC
	QString m_charset;
#endif
};

#endif /* _CONFIG_EX_H_ */

