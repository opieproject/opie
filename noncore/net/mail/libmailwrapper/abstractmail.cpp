#include "abstractmail.h"
#include "imapwrapper.h"
#include "pop3wrapper.h"
#include "nntpwrapper.h"
#include "mhwrapper.h"
#include "mailtypes.h"

#include <qfile.h>
#include <qtextstream.h>
#include <stdlib.h>
#include <libetpan/mailmime_content.h>
#include <libetpan/mailmime.h>

AbstractMail* AbstractMail::getWrapper(IMAPaccount *a)
{
    return new IMAPwrapper(a);
}

AbstractMail* AbstractMail::getWrapper(POP3account *a)
{
    return new POP3wrapper(a);
}

AbstractMail* AbstractMail::getWrapper(NNTPaccount *a)
{
    return new NNTPwrapper(a);
}

AbstractMail* AbstractMail::getWrapper(const QString&a,const QString&name)
{
    return new MHwrapper(a,name);
}

AbstractMail* AbstractMail::getWrapper(Account*a)
{
    if (!a) return 0;
    switch (a->getType()) {
        case MAILLIB::A_IMAP:
            return new IMAPwrapper((IMAPaccount*)a);
            break;
        case MAILLIB::A_POP3:
            return new POP3wrapper((POP3account*)a);
            break;
        case MAILLIB::A_NNTP:
            return new NNTPwrapper((NNTPaccount*)a);
            break;
        default:
            return 0;
    }
}

encodedString* AbstractMail::decode_String(const encodedString*text,const QString&enc)
{
    qDebug("Decode string start");
    char*result_text;
    size_t index = 0;
    /* reset for recursive use! */
    size_t target_length = 0;
    result_text = 0;
    int mimetype = MAILMIME_MECHANISM_7BIT;
    if (enc.lower()=="quoted-printable") {
        mimetype = MAILMIME_MECHANISM_QUOTED_PRINTABLE;
    } else if (enc.lower()=="base64") {
        mimetype = MAILMIME_MECHANISM_BASE64;
    } else if (enc.lower()=="8bit") {
        mimetype = MAILMIME_MECHANISM_8BIT;
    } else if (enc.lower()=="binary") {
        mimetype = MAILMIME_MECHANISM_BINARY;
    }

    int err = mailmime_part_parse(text->Content(),text->Length(),&index,mimetype,
    &result_text,&target_length);

    encodedString* result = new encodedString();
    if (err == MAILIMF_NO_ERROR) {
        result->setContent(result_text,target_length);
    }
    qDebug("Decode string finished");
    return result;
}

QString AbstractMail::convert_String(const char*text)
{
    size_t index = 0;
    char*res = 0;

    /* attention - doesn't work with arm systems! */
    int err = mailmime_encoded_phrase_parse("iso-8859-1",
        text, strlen(text),&index, "iso-8859-1",&res);
    if (err != MAILIMF_NO_ERROR) {
        if (res) free(res);
        return QString(text);
    }
    if (res) {
        QString result(res);
        free(res);
        return result;
    }
    return QString(text);
}

/* cp & paste from launcher */
QString AbstractMail::gen_attachment_id()
{
    QFile file( "/proc/sys/kernel/random/uuid" );
    if (!file.open(IO_ReadOnly ) )
        return QString::null;

    QTextStream stream(&file);

    return "{" + stream.read().stripWhiteSpace() + "}";
}

int AbstractMail::createMbox(const QString&,const Folder*,const QString& delemiter,bool)
{
    return 0;
}

QString AbstractMail::defaultLocalfolder()
{
    QString f = getenv( "HOME" );
    f += "/Applications/opiemail/localmail";
    return f;
}

QString AbstractMail::draftFolder()
{
    return QString("Drafts");
}

/* temporary - will be removed when implemented in all classes */
void AbstractMail::deleteMails(const QString &,QList<RecMail> &)
{
}

void AbstractMail::mvcpAllMails(Folder*fromFolder,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    QList<RecMail> t;
    listMessages(fromFolder->getName(),t);
    encodedString*st = 0;
    while (t.count()>0) {
        RecMail*r = t.at(0);
        st = fetchRawBody(*r);
        if (st) {
            targetWrapper->storeMessage(st->Content(),st->Length(),targetFolder);
            delete st;
        }
        t.removeFirst();
    }
    if (moveit) {
        deleteAllMail(fromFolder);
    }
}

void AbstractMail::mvcpMail(const RecMail&mail,const QString&targetFolder,AbstractMail*targetWrapper,bool moveit)
{
    encodedString*st = 0;
    st = fetchRawBody(mail);
    if (st) {
        targetWrapper->storeMessage(st->Content(),st->Length(),targetFolder);
        delete st;
    }
    if (moveit) {
        deleteMail(mail);
    }
}
