/*
** $Id: qsettings.cpp,v 1.2 2004-03-01 20:04:34 chicken Exp $
*/

#include "qsettings.h"
#include <stdio.h>
#include <qfile.h>
#include <qtextstream.h>


QSettings::QSettings(const QString &_fn)
{
	// read the prefs from the file
	fn = _fn;
	
	QFile f(_fn);
    if ( f.open(IO_ReadOnly) ) {    // file opened successfully
        QTextStream t( &f );        // use a text stream
        QString s;
        while ( !t.eof() ) {        // until end of file...
            s = t.readLine();       // line of text excluding '\n'
			char buf[256];
			sprintf (buf, "%s", (const char *) s);
			int pos = s.find (" = ");
			QString key = s.left (pos);
			QString val = s.right (s.length() - pos - 3);
			writeEntry (key, val);
			
			sprintf (buf, "%s|%s", (const char *)key, (const char *)val);
        }
        f.close();
    }
	

}

QSettings::~QSettings()
{
	// write out the prefs to the file
	QAsciiDictIterator <QString> it( prefs ); // iterator for dict
	QFile f(fn);
	f.open(IO_WriteOnly);
	QTextStream ts( &f );

    while ( it.current() )
	{
	   QString *key = new QString(it.currentKey());
	   char buf[256];
	   sprintf	 (buf, "%s", (const char *) *( it.current()));
	   QString *val = new QString(buf);
	   sprintf (buf, "%s %s", (const char *)*key, (const char *)*val);
	   ts << *key << " = " << *val << endl;
       ++it;
	}
	
	f.close();
	prefs.clear();
}

void QSettings::insertSearchPath (System sys, const QString &str)
{
	fn = str;
}

QString QSettings::readEntry (const QString &key, const QString &def)
{
	
	QString *s = prefs.find((const char *)key);
	if (!s)
		return def;
	else 
		return *s;
	
}

int     QSettings::readNumEntry (const QString &key, int def)
{
	QString *s = prefs[key];
	if (!s) return def;
    return s->toInt();
}

bool    QSettings::readBoolEntry (const QString &key, bool def)
{
	QString *s = prefs[key];
	if (!s) return def;
	if (*s == "1")
		return true;
	else
		return false;
}

bool    QSettings::writeEntry (const QString &key, int val)
{
	char buf[64];
	sprintf (buf, "%d", val);
	QString *v = new QString(buf);
    prefs.replace ((const char *)key, v);
	return true;
}

bool    QSettings::writeEntry (const QString &key, bool val)
{
	QString *v;
	if (val)
	   v = new QString("1");
	else
       v = new QString("0");
    prefs.replace ((const char *)key, v);
	return true;
}

bool    QSettings::writeEntry (const QString &key, const QString &val)
{
	QString *v = new QString (val);
	prefs.replace ((const char *)key, v);
	return true;
}

bool    QSettings::writeEntry (const QString &key, const char *val)
{
	QString *v = new QString (val);
	prefs.replace ((const char *)key, v);
	return true;
}

bool    QSettings::removeEntry (const QString &key)
{
	prefs.remove (key);
	return true;
}

QStringList QSettings::entryList (const QString &key) const
{
	QAsciiDictIterator <QString> it( prefs ); // iterator for dict
	QStringList list;

    while ( it.current() )
	{
	   char buf[512];
	   sprintf	 (buf, "%s", (const char *) *( it.current()));
	   QString *val = new QString(buf);
       sprintf(buf, "%s -> %s\n", it.currentKey(), (const char *)*val );
	   QString *cat = new QString(it.currentKey());
       if (cat->contains("-field", FALSE))
	      list.append (it.currentKey());
       ++it;
    
	}
	return list;
}

