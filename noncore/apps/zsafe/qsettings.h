/*
** $Id: qsettings.h,v 1.1 2003-07-22 19:23:20 zcarsten Exp $
*/

#ifndef _QSETTINGS_H_
#define _QSETTINGS_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qasciidict.h>

// class to hold one category item

class QSettings
{
public:
        enum Format {
		Native = 0,
		Ini
	};
	enum System {
		Unix = 0,
		Windows,
		Mac
	};
	enum Scope {
		User,
		Global
	};
	
	QSettings(const QString &);
	~QSettings();

	void insertSearchPath (System, const QString &);

	QString readEntry (const QString &, const QString &def = QString::null);
	int     readNumEntry (const QString &, int def = 0);
	bool    readBoolEntry (const QString &, bool def = 0);

	bool    writeEntry (const QString &, int);
	bool    writeEntry (const QString &, bool);
	bool    writeEntry (const QString &, const QString &);
	bool    writeEntry (const QString &, const char *);

	bool    removeEntry (const QString &);

	QStringList entryList (const QString &) const;

protected:

private:
	QAsciiDict <QString> prefs; // key, value dictonary
	QString fn; // preference filename

};
#endif // _QSETTINGS_H_
