#ifndef __GENERATE_MAIL_H
#define __GENERATE_MAIL_H

#include <qpe/applnk.h>

#include <qobject.h>
#include <libetpan/clist.h>

#include <opie2/osmartpointer.h>

class Mail;
class RecMail;
class Attachment;
struct mailimf_fields;
struct mailimf_field;
struct mailimf_mailbox;
struct mailmime;
struct mailimf_address_list;
class progressMailSend;
struct mailsmtp;

class Generatemail : public QObject
{
    Q_OBJECT
public:
    Generatemail();
    virtual ~Generatemail();

protected:
    static void addRcpts( clist *list, mailimf_address_list *addr_list );
    static char *getFrom( mailmime *mail );
    static char *getFrom( mailimf_field *ffrom);
    static mailimf_field *getField( mailimf_fields *fields, int type );
    mailimf_address_list *parseAddresses(const QString&addr );
    void addFileParts( mailmime *message,const QList<Attachment>&files );
    mailmime *buildFilePart(const QString&filename,const QString&mimetype,const QString&content);
    mailmime *buildTxtPart(const QString&str );
    mailimf_mailbox *newMailbox(const QString&name,const QString&mail );
    mailimf_fields *createImfFields(const Opie::OSmartPointer<Mail> &mail );
    mailmime *createMimeMail(const Opie::OSmartPointer<Mail>&mail );
    clist *createRcptList( mailimf_fields *fields );

    static const char* USER_AGENT;
};

#endif
