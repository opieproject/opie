#include "storemail.h"
#include "mailwrapper.h"
#include "settings.h"
#include "abstractmail.h"

#include <libetpan/libetpan.h>

#include <qstring.h>

#include <stdlib.h>

Storemail::Storemail(Account*aAccount,const QString&aFolder)
    : Generatemail()
{
    wrapper = 0;
    m_Account = aAccount;
    m_tfolder = aFolder;
    wrapper = AbstractMail::getWrapper(m_Account);
    if (wrapper) {
        wrapper->createMbox(m_tfolder);
    }
}

Storemail::Storemail(const QString&dir,const QString&aFolder)
    : Generatemail()
{
    wrapper = 0;
    m_Account = 0;
    m_tfolder = aFolder;
    wrapper = AbstractMail::getWrapper(dir);
    if (wrapper) {
        wrapper->createMbox(m_tfolder);
    }
}

Storemail::Storemail(const QString&aFolder)
    : Generatemail()
{
    wrapper = 0;
    m_Account = 0;
    m_tfolder = aFolder;
    wrapper = AbstractMail::getWrapper(AbstractMail::defaultLocalfolder());
    if (wrapper) {
        wrapper->createMbox(m_tfolder);
    }
}

Storemail::~Storemail()
{
}

int Storemail::storeMail(const Opie::osmart_pointer<Mail>&mail)
{
    if (!wrapper) return 0;
    int ret = 1;

    mailmime * mimeMail = 0;
    mimeMail = createMimeMail(mail );
    if ( mimeMail == NULL ) {
        qDebug( "storeMail: error creating mime mail" );
        return 0;
    }
    char *data;
    size_t size;
    data = 0;

    mailmessage * msg = 0;
    msg = mime_message_init(mimeMail);
    mime_message_set_tmpdir(msg,getenv( "HOME" ));
    int r = mailmessage_fetch(msg,&data,&size);
    mime_message_detach_mime(msg);
    mailmessage_free(msg);
    msg = 0;
    if (r != MAIL_NO_ERROR || !data) {
        qDebug("Error fetching mime...");
        ret = 0;
    }

    if (ret) {
        wrapper->storeMessage(data,size,m_tfolder);
    }

    if (data) {
        free(data);
    }
    if (mimeMail) {
        mailmime_free( mimeMail );
    }
    return ret;
}
