#include "nationalcfg.h"

#include <opie2/odebug.h>

#include <qfile.h>

static QString _key_desc="description";
static QString _key_doc="nationaldays";
static QString _key_list="entries";
static QString _key_entry="entry";
static QString _content_name="name";
static QString _content_date="date";

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
    _content.clear();
    QFile *f=new QFile(_path);
    if (!f) {
        oerr << "Could not open file" << oendl;
        return false;
    }
    QXmlInputSource is(*f);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);

    err = "";
    bool ret = reader.parse(is);
    if (err.length()>0)
        odebug << "Errors: " << err << oendl;
    return ret;
}

const tholidaylist&NHcfg::days()const
{
    return _content;
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
    bool ret = false;
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
        ret = setName(attr);
        return ret;
    }
    if (stage<2) {return false;}
    if (name==_key_list) {stage=3;return true;}
    if (stage<3) {return false;}
    return parsevalue(name,attr);
}

bool NHcfg::parsevalue(const QString&name,const QXmlAttributes&attr)
{
    int nindex = attr.index(_content_name);
    int dindex = attr.index(_content_date);
    if (name != _key_entry) {err = "Not a valid entry"; return false;}
    if (dindex == -1 || nindex == -1) {err = QString("Listentry %i is invalid").arg(1);return false;}
    QString txt = attr.value(nindex);
    QString dstring = attr.value(dindex);
    QStringList e = QStringList::split("-",dstring);
    if (e.count()!=2){err=QString("Datestring %1 is invalid").arg(dstring);return false;}
    QDate d(0,e[0].toInt(),e[1].toInt());
    odebug << "Found entry \"" << txt<<"\" on "<<d<<oendl;
    _content[d].append(txt);
    return true;
}

bool NHcfg::endElement(const QString&, const QString&,const QString& name)
{
    return true;
}

const QString&NHcfg::errorString()const
{
    return err;
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
