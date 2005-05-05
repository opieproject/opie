
#ifndef USEQPE

#include "preferences.h"

#include <qfile.h>
#include <qtextstream.h>

Config::Config(const QString& _fn) : fname(_fn)
{
//    qDebug("Config::Config:%s", (const char*)fname);
    QFile fl(fname);
    if (fl.open(IO_ReadOnly))
    {
	QTextStream t(&fl);
	QString key, value;
	while (!t.eof())
	{
	    QString data = t.readLine();
	    int colon = data.find(':');
	    if (colon > 0)
	    {
		QString key = data.left(colon);
		QString value = data.right(data.length()-colon-1);
		values[key] = value;
	    }
	}
	fl.close();
    }
//    read entries into values
}
Config::~Config()
{
//    qDebug("Config::~Config:%s", (const char*)fname);
    QFile fl(fname);
    if (fl.open(IO_WriteOnly))
    {
	QTextStream t(&fl);
	for (QMap<QString,QString>::Iterator iter = values.begin();
	     iter != values.end();
	     iter++)
	{
	    t << iter.key() << ':' << iter.data() << '\n';
	}
	fl.close();
    }
}
QString Config::readEntry(const QString& key, const QString& deflt)
{
    QMap<QString,QString>::Iterator iter = values.find(key);
    if (iter != values.end())
    {
	return iter.data();
    }
    else
    {
	return deflt;
    }
}
bool Config::readBoolEntry(const QString& key, const bool deflt)
{
    bool ok;
    QMap<QString,QString>::Iterator iter = values.find(key);
    if (iter != values.end())
    {
	int ret = iter.data().toInt(&ok);
	return ((ok) ? !!ret : deflt);
    }
    else
    {
	return deflt;
    }
}
int Config::readNumEntry(const QString& key, const int deflt)
{
    bool ok;
    QMap<QString,QString>::Iterator iter = values.find(key);
    if (iter != values.end())
    {
	int ret = iter.data().toInt(&ok);
	return ((ok) ? ret : deflt);
    }
    else
    {
	return deflt;
    }
}
void Config::writeEntry(const QString& key, const QString& value)
{
    values[key] = value;
}

void Config::writeEntry(const QString& key, const bool value)
{
    values[key] = (value) ? "1" : "0";
}

void Config::writeEntry(const QString& key, const int value)
{
    QString rhs;
    rhs.setNum(value);
    values[key] = rhs;
}

#endif
