#include "mailtypes.h"
#include <stdlib.h>

RecMail::RecMail()
    :subject(""),date(""),from(""),mbox(""),msg_id(""),msg_number(0),msg_size(0),msg_flags(7)
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

RecMail::~RecMail()
{
    wrapper = 0;
}

void RecMail::copy_old(const RecMail&old)
{
    subject = old.subject;
    date = old.date;
    mbox = old.mbox;
    msg_id = old.msg_id;
    msg_size = old.msg_size;
    msg_number = old.msg_number;
    from = old.from;
    msg_flags = old.msg_flags;
    to = old.to;
    cc = old.cc;
    bcc = old.bcc;
    wrapper = old.wrapper;
}

void RecMail::init()
{
    to.clear();
    cc.clear();
    bcc.clear();
    wrapper = 0;
}

void RecMail::setWrapper(AbstractMail*awrapper)
{
    wrapper = awrapper;
}

AbstractMail* RecMail::Wrapper()
{
    return wrapper;
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
    : m_type(""),m_subtype(""),m_identifier(""),m_encoding(""),m_description(""),m_lines(0),m_size(0)
{
    m_Parameters.clear();
    m_poslist.clear();
}

RecPart::~RecPart()
{
}

void RecPart::setSize(unsigned int size)
{
    m_size = size;
}
    
const unsigned int RecPart::Size()const
{
    return m_size;
}

void RecPart::setLines(unsigned int lines)
{
    m_lines = lines;
}
    
const unsigned int RecPart::Lines()const
{
    return m_lines;
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

const QString& RecPart::Description()const
{
    return m_description;
}

void RecPart::setDescription(const QString&desc)
{
    m_description = desc;
}
    
void RecPart::setParameters(const part_plist_t&list)
{
    m_Parameters = list;
}

const part_plist_t& RecPart::Parameters()const
{
    return m_Parameters;
}

void RecPart::addParameter(const QString&key,const QString&value)
{
    m_Parameters[key]=value;
}

const QString RecPart::searchParamter(const QString&key)const
{
    QString value("");
    part_plist_t::ConstIterator it = m_Parameters.find(key);
    if (it != m_Parameters.end()) {
        value = it.data();
    }
    return value;
}

void RecPart::setPositionlist(const QValueList<int>&poslist)
{
    m_poslist = poslist;
}

const QValueList<int>& RecPart::Positionlist()const
{
    return m_poslist;
}

RecBody::RecBody()
    : m_BodyText(),m_PartsList(),m_description()
{
    m_PartsList.clear();
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

void RecBody::setParts(const QValueList<RecPart>&parts)
{
    m_PartsList.clear();
    m_PartsList = parts;
}

const QValueList<RecPart>& RecBody::Parts()const
{
    return m_PartsList;
}

void RecBody::addPart(const RecPart& part)
{
    m_PartsList.append(part);
}

void RecBody::setDescription(const RecPart&des)
{
    m_description = des;
}

const RecPart& RecBody::Description()const
{
    return m_description;
}

/* handling encoded content */
encodedString::encodedString()
{
    init();
}

encodedString::encodedString(const char*nContent,unsigned int nSize)
{
    init();
    setContent(nContent,nSize);
}

encodedString::encodedString(char*nContent,unsigned int nSize)
{
    init();
    setContent(nContent,nSize);
}

encodedString::encodedString(const encodedString&old)
{
    init();
    copy_old(old);
    qDebug("encodedeString: copy constructor!");
}

encodedString& encodedString::operator=(const encodedString&old)
{
    init();
    copy_old(old);
    qDebug("encodedString: assign operator!");
    return *this;
}

encodedString::~encodedString()
{
    clean();
}

void encodedString::init()
{
    content = 0;
    size = 0;
}

void encodedString::clean()
{
    if (content) {
        free(content);
    }
    content = 0;
    size = 0;
}

void encodedString::copy_old(const encodedString&old)
{
    clean();
    if (old.size>0 && old.content) {
        content = (char*)malloc(old.size*sizeof(char));
        memcpy(content,old.content,size);
        size = old.size;
    }
}

const char*encodedString::Content()const
{
    return content;
}

const int encodedString::Length()const
{
    return size;
}

void encodedString::setContent(const char*nContent,int nSize)
{
    if (nSize>0 && nContent) {
        content = (char*)malloc(nSize*sizeof(char));
        memcpy(content,nContent,nSize);
        size = nSize;    
    }
}

void encodedString::setContent(char*nContent,int nSize)
{
    content = nContent;
    size = nSize;
}
