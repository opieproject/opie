/*
 * GPL from TT
 */

#ifndef OPIE_RECUR_H
#define OPIE_RECUR_H

#include <sys/types.h>

#include <qdatetime.h>
#include <qvaluelist.h>


class ORecur {
public:
    typedef QValueList<QDate> ExceptionList;
    enum RepeatType{ NoRepeat = -1, Daily, Weekly, MonthlyDay,
                     MonthlyDate, Yearly };
    enum Days { MON = 0x01, TUE = 0x02, WED = 0x04, THU = 0x08,
                FRI = 0x10, SAT = 0x20, SUN = 0x40 };
    ORecur();
    ORecur( const ORecur& );
    ~ORecur();

    ORecur &operator=( const ORecur& );
    bool operator==(const ORecur& )const;

    bool doesRecur()const;
    /* if it recurrs on that day */
    bool doesRecur( const QDate& );
    RepeatType type()const;
    int frequency()const;
    int position()const;
    char days()const;
    bool hasEndDate()const;
    QDate start()const;
    QDate endDate()const;
    QDateTime createdDateTime()const;
    /**
     * starting on monday=0, sunday=6
     * for convience
     */
    bool repeatOnWeekDay( int day )const;

    /**
     * FromWhereToStart is not included!!!
     */
    bool nextOcurrence( const QDate& FromWhereToStart, QDate &recurDate );

    /**
     * The module this ORecur belongs to
     */
    QString service()const;

    /*
     * reference to the exception list
     */
    ExceptionList &exceptions();

    /**
     * the current repetition
     */
    int repetition()const;

    void setType( const RepeatType& );
    void setFrequency( int freq );
    void setPosition( int pos );
    void setDays( char c);
    void setEndDate( const QDate& dt );
    void setStart( const QDate& dt );
    void setCreatedDateTime( const QDateTime& );
    void setHasEndDate( bool b );
    void setRepitition(int );

    void setService( const QString& ser );

    /* almost internal */
    QString toString()const;
private:
    bool p_nextOccurrence( const QDate& from, QDate& next );
    void deref();
    inline void checkOrModify();


    class Data;
    Data* data;
    class ORecurPrivate;
    ORecurPrivate *d;
};

#endif
