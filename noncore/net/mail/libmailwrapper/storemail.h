#ifndef __STORE_MAIL_H
#define __STORE_MAIL_H

#include <qpe/applnk.h>

#include "generatemail.h"

class Account;
class Mail;
class AbstractMail;

class Storemail : public Generatemail
{
    Q_OBJECT
public:
    Storemail(Account*aAccount,const QString&aFolder);
    Storemail(const QString&dir,const QString&aFolder);
    Storemail(const QString&aFolder);
    virtual ~Storemail();

    int storeMail(const Mail&mail);

protected:
    Account* m_Account;
    QString m_tfolder;
    AbstractMail*wrapper;
};

#endif
