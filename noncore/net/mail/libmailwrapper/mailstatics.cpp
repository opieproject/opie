#include "mailstatics.h"
#include <libetpan/libetpan.h>
//#include <qpe/timestring.h>

QDateTime MailStatics::parseDateTime(const mailimf_date_time *date,int&offset )
{
    offset = 0;
    if (!date) {
        return QDateTime();
    }
    QDateTime da(QDate(date->dt_year,date->dt_month,date->dt_day),QTime(date->dt_hour,date->dt_min,date->dt_sec));
    offset = date->dt_zone;
    return da;
}

QDateTime MailStatics::parseDateTime(const char*date,int&offset)
{
    mailimf_date_time * date_time=0;
    QDateTime d;
    size_t cur_tok = 0;
    if (!date) return d;
    int r = mailimf_date_time_parse(date,strlen(date),&cur_tok,&date_time);
    if (r==MAILIMF_NO_ERROR) {
        d = parseDateTime(date_time,offset);
    }
    if (date_time) {
        mailimf_date_time_free(date_time);
    }
    return d;
}
