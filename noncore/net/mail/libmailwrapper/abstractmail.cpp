#include "abstractmail.h"
#include "imapwrapper.h"
#include "pop3wrapper.h"

AbstractMail* AbstractMail::getWrapper(IMAPaccount *a)
{
    return new IMAPwrapper(a);
}

AbstractMail* AbstractMail::getWrapper(POP3account *a)
{
    return new POP3wrapper(a);
}
