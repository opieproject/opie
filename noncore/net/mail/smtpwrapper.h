#ifndef SMTPwrapper_H
#define SMTPwrapper_H

#include <qpe/applnk.h>

#include <qbitarray.h>
#include <qdatetime.h>
#include <libetpan/clist.h>

#include "settings.h"

class Mail;
class Attachment;
struct mailimf_fields;
struct mailimf_field;
struct mailimf_mailbox;
struct mailmime;
struct mailimf_address_list;

class SMTPwrapper : public QObject
{
    Q_OBJECT

public:
    SMTPwrapper( Settings *s ); 
    virtual ~SMTPwrapper(){}
    void sendMail(const Mail& mail );

protected:
    mailimf_mailbox *newMailbox(const QString&name,const QString&mail );
    mailimf_fields *createImfFields(const Mail &mail );
    mailmime *createMimeMail(const Mail&mail );
    
    mailimf_address_list *parseAddresses(const QString&addr );
    void addFileParts( mailmime *message,const QList<Attachment>&files );
    mailmime *buildTxtPart(const QString&str );
    mailmime *buildFilePart(const QString&filename,const QString&mimetype );
    void smtpSend( mailmime *mail );
    mailimf_field *getField( mailimf_fields *fields, int type );
    clist *createRcptList( mailimf_fields *fields );
    char *getFrom( mailmime *mail );
    SMTPaccount *getAccount(const QString&from );
    void writeToFile(const QString&file, mailmime *mail );
    void readFromFile(const QString&file, char **data, size_t *size );
    
    static QString mailsmtpError( int err );
    static QString getTmpFile();
    static void progress( size_t current, size_t maximum );
    static void addRcpts( clist *list, mailimf_address_list *addr_list );
    Settings *settings;
};

#endif
