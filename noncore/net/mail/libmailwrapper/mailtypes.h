#ifndef __MAIL_TYPES_H
#define __MAIL_TYPES_H

#define FLAG_ANSWERED 0
#define FLAG_FLAGGED  1
#define FLAG_DELETED  2
#define FLAG_SEEN     3
#define FLAG_DRAFT    4
#define FLAG_RECENT   5

#include <qlist.h>
#include <qbitarray.h>
#include <qstring.h>
#include <qstringlist.h>

/* a class to describe mails in a mailbox */
/* Attention!
   From programmers point of view it would make sense to
   store the mail body into this class, too.
   But: not from the point of view of the device. 
   Mailbodies can be real large. So we request them when 
   needed from the mail-wrapper class direct from the server itself
   (imap) or from a file-based cache (pop3?)
   So there is no interface "const QString&body()" but you should
   make a request to the mailwrapper with this class as parameter to
   get the body. Same words for the attachments.
*/
class RecMail
{
public:
    RecMail();
    RecMail(const RecMail&old);
    virtual ~RecMail(){}

    const int getNumber()const{return msg_number;}
    void setNumber(int number){msg_number=number;}
    const QString&getDate()const{ return date; }
    void setDate( const QString&a ) { date = a; }
    const QString&getFrom()const{ return from; }
    void setFrom( const QString&a ) { from = a; }
    const QString&getSubject()const { return subject; }
    void setSubject( const QString&s ) { subject = s; }
    const QString&getMbox()const{return mbox;}
    void setMbox(const QString&box){mbox = box;}
    void setMsgid(const QString&id){msg_id=id;}
    const QString&Msgid()const{return msg_id;}
    void setReplyto(const QString&reply){replyto=reply;}
    const QString&Replyto()const{return replyto;}


    void setTo(const QStringList&list);
    const QStringList&To()const;
    void setCC(const QStringList&list);
    const QStringList&CC()const;
    void setBcc(const QStringList&list);
    const QStringList&Bcc()const;
    const QBitArray&getFlags()const{return msg_flags;}
    void setFlags(const QBitArray&flags){msg_flags = flags;}

protected:
    QString subject,date,from,mbox,msg_id,replyto;
    int msg_number;
    QBitArray msg_flags;
    QStringList to,cc,bcc;
    void init();
    void copy_old(const RecMail&old);
};

class RecPart
{
protected:
    QString m_type,m_subtype,m_identifier,m_encoding;
public:
    RecPart();
    RecPart(const QString&identifier,const QString&type="",const QString&subtype="",const QString&encoding="BASE64");
    virtual ~RecPart();
    
    const QString&Type()const;
    void setType(const QString&type);
    const QString&Subtype()const;
    void setSubtype(const QString&subtype);
    const QString&Identifier()const;
    void setIdentifier(const QString&identifier);
    const QString&Encoding()const;
    void setEncoding(const QString&encoding);
};

class RecBody
{
protected:
    QString m_BodyText;
    QList<RecPart> m_PartsList;

public:
    RecBody();
    virtual ~RecBody();
    void setBodytext(const QString&);
    const QString& Bodytext()const;
    
    void setParts(const QList<RecPart>&parts);
    const QList<RecPart>& Parts()const;
    void addPart(const RecPart&part);
};

#endif
