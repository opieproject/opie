#ifndef MAILWRAPPER_H
#define MAILWRAPPER_H

#include <qpe/applnk.h>

#include <libetpan/mailmime.h>
#include <libetpan/mailimf.h>
#include <libetpan/mailsmtp.h>
#include <libetpan/mailstorage.h>
#include <libetpan/maildriver.h>
#include <qbitarray.h>
#include <qdatetime.h>

#include "settings.h"

class Attachment
{
public:
    Attachment( DocLnk lnk ); 
    virtual ~Attachment(){}
    const QString getFileName()const{ return doc.file(); }
    const QString getName()const{ return doc.name(); }
    const QString getMimeType()const{ return doc.type(); }
    const QPixmap getPixmap()const{ return doc.pixmap(); }
    const int getSize()const { return size; }
    DocLnk  getDocLnk() { return doc; }
    
protected:
    DocLnk doc;
    int size;

};

class Mail
{
public:
    Mail();
    /* Possible that this destructor must not be declared virtual
     * 'cause it seems that it will never have some child classes.
     * in this case this object will not get a virtual table -> memory and
     * speed will be a little bit better?
     */
    virtual ~Mail(){}
    void addAttachment( Attachment *att ) { attList.append( att ); }
    const QList<Attachment>& getAttachments()const { return attList; }
    void removeAttachment( Attachment *att ) { attList.remove( att ); } 
    const QString&getName()const { return name; }
    void setName( QString s ) { name = s; }
    const QString&getMail()const{ return mail; }
    void setMail( const QString&s ) { mail = s; }
    const QString&getTo()const{ return to; }
    void setTo( const QString&s ) { to = s; }
    const QString&getCC()const{ return cc; }
    void setCC( const QString&s ) { cc = s; }
    const QString&getBCC()const { return bcc; }
    void setBCC( const QString&s ) { bcc = s; }
    const QString&getMessage()const { return message; }
    void setMessage( const QString&s ) { message = s; }
    const QString&getSubject()const { return subject; }
    void setSubject( const QString&s ) { subject = s; }
    const QString&getReply()const{ return reply; }
    void setReply( const QString&a ) { reply = a; }

private:
    QList<Attachment> attList;
    QString name, mail, to, cc, bcc, reply, subject, message;
};

class Folder : public QObject
{
    Q_OBJECT

public:
    Folder( const QString&init_name );
    const QString&getDisplayName()const { return nameDisplay; }
    const QString&getName()const { return name; }
    virtual bool may_select()const{return true;};    

private:
    QString nameDisplay, name;
    
};

class IMAPFolder : public Folder
{
    public:
        IMAPFolder( QString name,bool select=true ) : Folder( name ),m_MaySelect(select) {}
        virtual bool may_select()const{return m_MaySelect;}
    private:
        bool m_MaySelect;
};

class MailWrapper : public QObject
{
    Q_OBJECT

public:
    MailWrapper( Settings *s ); 
    void sendMail( Mail mail );

private:
    mailimf_mailbox *newMailbox(const QString&name,const QString&mail );
    mailimf_address_list *parseAddresses(const QString&addr );
    mailimf_fields *createImfFields( Mail *mail );
    mailmime *buildTxtPart( QString str );
    mailmime *buildFilePart( QString filename, QString mimetype );
    void addFileParts( mailmime *message, QList<Attachment> files );
    mailmime *createMimeMail( Mail *mail );
    void smtpSend( mailmime *mail );
    mailimf_field *getField( mailimf_fields *fields, int type );
    clist *createRcptList( mailimf_fields *fields );
    char *getFrom( mailmime *mail );
    SMTPaccount *getAccount( QString from );
    void writeToFile( QString file, mailmime *mail );
    void readFromFile( QString file, char **data, size_t *size );
    static QString mailsmtpError( int err );
    static QString getTmpFile();

    Settings *settings;
        
};

#endif
