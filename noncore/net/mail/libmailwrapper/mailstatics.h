#ifndef __MAIL_STATICS_H
#define __MAIL_STATICS_H

#include <qdatetime.h>
#include <qstring.h>

struct mailimf_date_time;

class MailStatics
{
protected:
    static QString parseDateTime(const mailimf_date_time * date);
    static QString parseDateTime(const char*date);
    MailStatics(){};
    virtual ~MailStatics(){};
public:
};

#endif
