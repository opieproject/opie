#include "abstractmail.h"
#include "imapwrapper.h"
#include "pop3wrapper.h"

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

QString AbstractMail::decode_quoted_printable(const char*text)
{
    char*result_text;
    size_t index = 0;
    QString result = "";
    /* reset for recursive use! */
    size_t target_length = 0;
    result_text = 0;
    int err = mailmime_quoted_printable_body_parse(text,strlen(text),
            &index,&result_text,&target_length,0);
    if (result_text) {
        result = result_text;
        free(result_text);
    }
    return result;
}
