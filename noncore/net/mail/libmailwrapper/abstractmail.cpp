#include "abstractmail.h"
#include "imapwrapper.h"
#include "pop3wrapper.h"
#include "mailtypes.h"

#include <qstring.h>
#include <stdlib.h>
#include <libetpan/mailmime_content.h>

AbstractMail* AbstractMail::getWrapper(IMAPaccount *a)
{
    return new IMAPwrapper(a);
}

AbstractMail* AbstractMail::getWrapper(POP3account *a)
{
    return new POP3wrapper(a);
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
