#include "mailstatics.h"
#include <libetpan/libetpan.h>
#include <qpe/timestring.h>

QString MailStatics::parseDateTime(const mailimf_date_time *date )
{
    if (!date) return "";
    QDateTime da(QDate(date->dt_year,date->dt_month,date->dt_day),QTime(date->dt_hour,date->dt_min,date->dt_sec));
    QString timestring = TimeString::numberDateString(QDate(date->dt_year,date->dt_month,date->dt_day))+" ";
    timestring+=TimeString::timeString(QTime(date->dt_hour,date->dt_min,date->dt_sec))+" ";
    timestring.sprintf(timestring+" %+05i",date->dt_zone);
    return timestring;
}

QString MailStatics::parseDateTime(const char*date)
{
    mailimf_date_time * date_time;
    QString d = "";
    size_t cur_tok = 0;
    if (!date) return d;
    int r = mailimf_date_time_parse(date,strlen(date),&cur_tok,&date_time);
    if (r==MAILIMF_NO_ERROR) {
        d = parseDateTime(date_time);
    }
    if (date_time) {
        mailimf_date_time_free(date_time);
    }
    return d;
}