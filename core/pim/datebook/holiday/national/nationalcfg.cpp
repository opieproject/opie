#include "nationalcfg.h"

#include <opie2/odebug.h>

#include <qfile.h>

static QString _key_desc="description";
static QString _key_doc="nationaldays";
static QString _key_list="entries";
static QString _key_entry="entry";
static QString _key_calc="calculation";

static QString _content_type="type";
static QString _content_float="floating";
static QString _content_fix="fix";

static QString _content_name="name";
static QString _content_value="value";

static QString _content_date="date";
static QString _content_weekd="weekday";
static QString _content_offset="offset";
static QString _content_dayofmonth="dayofmonth";
static QString _content_datedep="datedep";
static QString _content_month="month";

NHcfg::NHcfg()
    :QXmlDefaultHandler(),err(""),_path("")
{
}

NHcfg::~NHcfg()
{
}

bool NHcfg::load(const QString&aPath)
{
    _path=aPath;
    stage = 0;
    QFile *f=new QFile(_path);
    if (!f) {
        oerr << "Could not open file" << oendl;
        return false;
    }
    QXmlInputSource is(*f);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    currentFixList.clear();
    currentFloatList.clear();
    counter = 0;
    level = 0;

    err = "";
    bool ret = reader.parse(is);
    if (err.length()>0)
        odebug << "Errors: " << err << oendl;
    return ret;
}

const tentrylist&NHcfg::fixDates()const
{
    return currentFixList;
}

const tentrylist&NHcfg::floatingDates()const
{
    return currentFloatList;
}

bool NHcfg::warning(const QXmlParseException& e)
{
        QString tmp;

        tmp.sprintf("%d: warning: %s\n", e.lineNumber(),
                (const char*) e.message().utf8());

        err += tmp;

        return true;
}

bool NHcfg::error(const QXmlParseException& e)
{
        QString tmp;

        tmp.sprintf("%d: error: %s\n", e.lineNumber(),
                (const char*) e.message().utf8());

        err += tmp;

        return true;
}

bool NHcfg::fatalError(const QXmlParseException& e)
{
        QString tmp;

        tmp.sprintf("%d: fatal error: %s\n", e.lineNumber(),
                (const char*) e.message().utf8());

        err += tmp;

        return false;
}

bool NHcfg::startElement(const QString&, const QString&,const QString& name, const QXmlAttributes& attr)
{
    if (name==_key_doc) {
        stage = 1;
        return true;
    }
    if (stage == 0) {
        err = "This is not a national holiday config file";
        return false;
    }
    if (name==_key_desc) {
        stage = 2;
        return setName(attr);
    }
    if (stage<2) return false;
    if (name==_key_list) {
        stage=3;
        return true;
    }

    if (stage<3) return false;

    if (name==_key_entry) {
        ++level;
        ++counter;
        m_currentEntry = NHentry();
  //      odebug << "Level == " << level << oendl;
    }
    if (name==_key_calc) {
        ++level;
//        odebug << "Level == " << level << oendl;
        return true;
    }
    if (level == 1) {
        return parsevalue(name,attr);
    } else if (level >= 2) {
        return parseCalc(name,attr);
    }
    return false;
}

bool NHcfg::setName(const QXmlAttributes&attr)
{
    int nindx = attr.index("value");
    if (nindx==-1) {
        return false;
    }
    _contentname = attr.value(nindx);
    return true;
}

bool NHcfg::parsevalue(const QString&name,const QXmlAttributes&attr)
{
    if (name != _key_entry && name != _key_calc) {err = QString("Not a valid entry (%1)").arg(name); return false;}

    int nindex = attr.index(_content_name);
    int dindex = attr.index(_content_date);
    int tindex = attr.index(_content_type);

    if (nindex == -1) {
        err = QString("Listentry %1 is invalid (name missing)").arg(counter);
        return false;
    }
    m_currentEntry.setName(attr.value(nindex));

    if (tindex != -1 && attr.value(tindex)==_content_float) {
        m_currentEntry.setType(NHentry::floating);
        return true;
    }

    if (dindex == -1) {
        err = QString("Listentry %1 is invalid ").arg(counter);return false;
    }

    QString txt = attr.value(nindex);
    QString dstring = attr.value(dindex);
    QStringList e = QStringList::split("-",dstring);
    if (e.count()!=2){err=QString("Datestring %1 is invalid (entry %2)").arg(dstring).arg(counter);return false;}
    QDate d(0,e[0].toInt(),e[1].toInt());
    m_currentEntry.setDate(d);
    return true;
}

bool NHcfg::parseCalc(const QString&name,const QXmlAttributes&attr)
{
    ++level;
    int vindex = attr.index(_content_value);
    if (vindex == -1) {
        err = QString("Value for element %1 on entry %2 is missing").arg(name).arg(counter);
        return false;
    }
    int what_c =0;
    QString what = name.lower();
    QString value;
    if (what == _content_date) {
        what_c = 1;
    } else if (what == _content_weekd) {
        what_c = 2;
    } else if (what == _content_offset) {
        what_c = 3;
    } else if (what == _content_dayofmonth) {
        what_c = 4;
    } else if (what == _content_datedep) {
        what_c = 5;
    } else if (what == _content_month) {
        what_c = 6;
    }
    if (what_c == 0) {
        err = QString("Unknown element %1 on entry %2").arg(name).arg(counter);
        return false;
    }
    value = attr.value(vindex).lower();
    QStringList e;
    QDate d;
    bool dotformat = false;
    switch (what_c) {
        case 1:
            if (value != "easter") {
                e = QStringList::split("-",value);
                if (e.count()!=2) {
                    e = QStringList::split(".",value);
                    dotformat = true;
                }
                if (e.count()!=2){err=QString("Datestring %1 is invalid (entry %2)").arg(value).arg(counter);return false;}
                if (!dotformat) {
                    d=QDate(0,e[0].toInt(),e[1].toInt());
                } else {
                    d=QDate(0,e[1].toInt(),e[0].toInt());
                }
            } else {
                d=QDate(9999,1,1);
            }
            m_currentEntry.setDate(d);
        break;
        case 2:
            m_currentEntry.setWeekday(value);
        break;
        case 3:
            m_currentEntry.setOffet(value.toInt());
        break;
        case 4:
            m_currentEntry.setDayofmonth(value);
        break;
        case 5:
            m_currentEntry.setDaydep(value);
        break;
        case 6:
            m_currentEntry.setMonth(value);
        break;
    }
    return true;
}

bool NHcfg::endElement(const QString&, const QString&,const QString& name)
{
//    odebug << "End element: " << name << oendl;
    if (name==_key_entry) {
        if (m_currentEntry.type()==NHentry::fix) {
            currentFixList.append(m_currentEntry);
        } else {
            currentFloatList.append(m_currentEntry);
//            odebug << "Floatlist count " << currentFloatList.count() << oendl;
        }
    } else if (name==_key_calc) {

    }
    if (stage>=3) {
        --level;
    }
//    odebug << "Level == " << level << oendl;
    return true;
}

QString NHcfg::errorString()
{
    return err;
}

NHentry::NHentry()
    :m_Type(fix)
{
    m_Offset=0;
}

NHentry::~NHentry()
{}

void NHentry::setName(const QString&aName)
{
    m_Name = aName;
}

const QString&NHentry::name()const
{
    return m_Name;
}

void NHentry::setType(NHentry::entry_type aType)
{
    m_Type = aType;
}

NHentry::entry_type NHentry::type()const
{
    return m_Type;
}

void NHentry::setDate(const QDate&aDate)
{
    m_Date = aDate;
}

const QDate&NHentry::date()const
{
    return m_Date;
}

void NHentry::setWeekday(const QString&aDay)
{
    m_Weekday = aDay;
}

const QString&NHentry::weekday()const
{
    return m_Weekday;
}

void NHentry::setDayofmonth(const QString&aDay)
{
    m_Dayofmonth = aDay;
}

const QString&NHentry::dayofmonth()const
{
    return m_Dayofmonth;
}

void NHentry::setDaydep(const QString&dep)
{
    m_Depth = dep;
}

const QString&NHentry::daydep()const
{
    return m_Depth;
}

void NHentry::setMonth(const QString&month)
{
    m_Month = month;
}

const QString&NHentry::month()const
{
    return m_Month;
}

void NHentry::setOffet(int aOffset)
{
    m_Offset = aOffset;
}

const int NHentry::offset()const
{
    return m_Offset;
}
