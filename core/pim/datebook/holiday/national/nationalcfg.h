#ifndef _NATIONAL_CFG_H
#define _NATIONAL_CFG_H

#include <qxml.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qdatetime.h>

typedef QMap<QDate,QStringList> tholidaylist;

class NHcfg:public QXmlDefaultHandler
{
public:
    NHcfg();
    virtual ~NHcfg();

    bool load(const QString&);
    const tholidaylist&days()const;

    virtual bool warning(const QXmlParseException& e);
    virtual bool error(const QXmlParseException& e);
    virtual bool fatalError(const QXmlParseException& e);
    virtual bool startElement(const QString&, const QString&,const QString& name, const QXmlAttributes& attr);
    virtual bool endElement(const QString&, const QString&,const QString& name);
    virtual const QString&errorString()const;

protected:
    tholidaylist _content;
    QString _contentname;
    QString err;
    QString _path;

    bool setName(const QXmlAttributes&);
    bool parsevalue(const QString&,const QXmlAttributes&);
    int stage;
//    int pos;
};

#endif
