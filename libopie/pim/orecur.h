/*
 * GPL from TT
 */

#ifndef OPIE_RECUR_H
#define OPIE_RECUR_H

#include <sys/types.h>

#include <qdatetime.h>



class ORecur {
public:
    enum RepeatType{ NoRepeat = -1, Daily, Weekly, MonthlyDay,
                     MonthlyDate, Yearly };
    enum Days { MON = 0x01, TUE = 0x02, WED = 0x04, THU = 0x08,
                FRI = 0x10, SAT = 0x20, SUN = 0x40 };
    ORecur();
    ORecur( const ORecur& );
    ~ORecur();

    ORecur &operator=( const ORecur& );
    bool operator==(const ORecur& )const;
    RepeatType type()const;
    int frequency()const;
    int position()const;
    char days()const;
    bool hasEndDate()const;
    QDate endDate()const;
    time_t endDateUTC()const;
    time_t createTime()const;

    void setType( const RepeatType& );
    void setFrequency( int freq );
    void setPosition( int pos );
    void setDays( char c);
    void setEndDate( const QDate& dt );
    void setEndDateUTC( time_t );
    void setCreateTime( time_t );
    void setHasEndDate( bool b );
private:
    void deref();
    inline void checkOrModify();


    class Data;
    Data* data;
    class ORecurPrivate;
    ORecurPrivate *d;
};

#endif
