#include "mailtypes.h"


RecMail::RecMail()
    :subject(""),date(""),from(""),mbox(""),msg_id(""),msg_number(0),msg_flags(7)
{
    init();
}

RecMail::RecMail(const RecMail&old)
    :subject(""),date(""),from(""),mbox(""),msg_id(""),msg_number(0),msg_flags(7)
{
    init();
    copy_old(old);
    qDebug("Copy constructor RecMail");
}

void RecMail::copy_old(const RecMail&old)
{
    subject = old.subject;
    date = old.date;
    mbox = old.mbox;
    msg_id = old.msg_id;
    msg_number = old.msg_number;
    from = old.from;
    msg_flags = old.msg_flags;
    to = old.to;
    cc = old.cc;
    bcc = old.bcc;
}

void RecMail::init()
{
    to.clear();
    cc.clear();
    bcc.clear();
}

void RecMail::setTo(const QStringList&list)
{
    to = list;
}

const QStringList&RecMail::To()const
{
    return to;
}

void RecMail::setCC(const QStringList&list)
{
    cc = list;
}

const QStringList&RecMail::CC()const
{
    return cc;
}

void RecMail::setBcc(const QStringList&list)
{
    bcc = list;
}

const QStringList& RecMail::Bcc()const
{
    return bcc;
}

RecPart::RecPart()
    : m_type(""),m_subtype(""),m_identifier(""),m_encoding("")
{
}

RecPart::RecPart(const QString&identifier,const QString&type,const QString&subtype,const QString&encoding)
    : m_type(type),m_subtype(subtype),m_identifier(identifier),m_encoding(encoding)
{
}

RecPart::~RecPart()
{
}

const QString& RecPart::Type()const
{
    return m_type;
}

void RecPart::setType(const QString&type)
{
    m_type = type;
}

const QString& RecPart::Subtype()const
{
    return m_subtype;
}

void RecPart::setSubtype(const QString&subtype)
{
    m_subtype = subtype;
}

const QString& RecPart::Identifier()const
{
    return m_identifier;
}

void RecPart::setIdentifier(const QString&identifier)
{
    m_identifier = identifier;
}

const QString& RecPart::Encoding()const
{
    return m_encoding;
}

void RecPart::setEncoding(const QString&encoding)
{
    m_encoding = encoding;
}

RecBody::RecBody()
    : m_BodyText(""),m_PartsList()
{
    m_PartsList.setAutoDelete(true);
}

RecBody::~RecBody()
{
}

void RecBody::setBodytext(const QString&bodyText)
{
    m_BodyText = bodyText;
}

const QString& RecBody::Bodytext()const
{
    return m_BodyText;
}

void RecBody::setParts(const QList<RecPart>&parts)
{
    m_PartsList = parts;
    m_PartsList.setAutoDelete(true);
}

const QList<RecPart>& RecBody::Parts()const
{
    return m_PartsList;
}

void RecBody::addPart(const RecPart& part)
{
    RecPart*p = new RecPart(part);
    m_PartsList.append(p);
}
