#include "useqpe.h"
#ifndef USEQPE
#ifndef __PREFERENCES_H
#define __PREFERENCES_H

#include <qstring.h>
#include <qmap.h>

class Config
{
    QMap<QString, QString> values;
    QString fname;
 public:
    Config(const QString& fname);
    ~Config();
	void setGroup(const QString& s) { }
    QString readEntry(const QString&, const QString&);
    bool readBoolEntry(const QString&, const bool);
    int readNumEntry(const QString&, const int);

    void writeEntry(const QString&, const QString&);
    void writeEntry(const QString&, const bool);
    void writeEntry(const QString&, const int);
};

#endif // PREFERENCES
#endif
