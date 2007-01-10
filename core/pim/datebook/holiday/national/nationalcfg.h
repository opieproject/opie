#ifndef _NATIONAL_CFG_H
#define _NATIONAL_CFG_H

#include <qxml.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qdatetime.h>

typedef QMap<QDate,QStringList> tholidaylist;

class NHentry
{
public:
    enum entry_type{fix,floating};
    NHentry();
    virtual ~NHentry();

    void setName(const QString&);
    const QString&name()const;
    void setType(entry_type);
    entry_type type()const;
    void setDate(const QDate&);
    const QDate&date()const;
    void setWeekday(const QString&);
    const QString&weekday()const;
    void setDayofmonth(const QString&);
    const QString&dayofmonth()const;
    void setDaydep(const QString&);
    const QString&daydep()const;
    void setMonth(const QString&);
    const QString&month()const;
    void setOffet(int);
    const int offset()const;

protected:
    entry_type m_Type;
    QString m_Name,m_Weekday,m_Dayofmonth,m_Depth,m_Month;
    QDate m_Date;
    int m_Offset;
};

typedef QValueList<NHentry> tentrylist;

class NHcfg:public QXmlDefaultHandler
{
public:
    NHcfg();
    virtual ~NHcfg();

    bool load(const QString&);
    const tentrylist&fixDates()const;
    const tentrylist&floatingDates()const;

    virtual bool warning(const QXmlParseException& e);
    virtual bool error(const QXmlParseException& e);
    virtual bool fatalError(const QXmlParseException& e);
    virtual bool startElement(const QString&, const QString&,const QString& name, const QXmlAttributes& attr);
    virtual bool endElement(const QString&, const QString&,const QString& name);
    virtual QString errorString()const;

protected:
    QString err,_contentname;
    QString _path;

    NHentry m_currentEntry;

    bool setName(const QXmlAttributes&);
    bool parsevalue(const QString&,const QXmlAttributes&);
    bool parseCalc(const QString&,const QXmlAttributes&);
    int stage,counter,level;
    tentrylist currentFloatList,currentFixList;
};

#endif
