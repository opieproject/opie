#ifndef __MAIL_STATICS_H
#define __MAIL_STATICS_H

#include <qdatetime.h>
#include <qstring.h>

struct mailimf_date_time;

class MailStatics
{
protected:
    static QDateTime parseDateTime(const mailimf_date_time * date,int&offset);
    static QDateTime parseDateTime(const char*date,int&offset);
    MailStatics(){};
    virtual ~MailStatics(){};
public:
};

#endif
