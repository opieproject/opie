
#ifndef OPIE_TODO_EVENT_H
#define OPIE_TODO_EVENT_H


#include <qarray.h>
#include <qmap.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include <qvaluelist.h>

#include <qpe/recordfields.h>
#include <qpe/palmtopuidgen.h>

#include <opie/opimrecord.h>


class OTodo : public  OPimRecord  {
public:
    typedef QValueList<OTodo> ValueList;
    enum RecordFields {
        Uid = Qtopia::UID_ID,
        Category = Qtopia::CATEGORY_ID,
        HasDate,
        Completed,
        Description,
        Summary,
        Priority,
        DateDay,
        DateMonth,
        DateYear,
        Progress,
        CrossReference,
        HasAlarmDateTime,
        AlarmDateTime
    };
 public:
    // priorities from Very low to very high
    enum TaskPriority { VeryHigh=1,  High,  Normal,  Low, VeryLow };

    /* Constructs a new ToDoEvent
       @param completed Is the TodoEvent completed
       @param priority What is the priority of this ToDoEvent
       @param category Which category does it belong( uid )
       @param summary A small summary of the todo
       @param description What is this ToDoEvent about
       @param hasDate Does this Event got a deadline
       @param date what is the deadline?
       @param uid what is the UUID of this Event
    **/
    OTodo( bool completed = false, int priority = Normal,
           const QStringList &category = QStringList(),
           const QString &summary = QString::null ,
           const QString &description = QString::null,
           ushort progress = 0,
           bool hasDate = false, QDate date = QDate::currentDate(),
           int uid = 0 /*empty*/ );

    OTodo( bool completed, int priority,
           const QArray<int>& category,
           const QString& summary = QString::null,
           const QString& description = QString::null,
           ushort progress = 0,
           bool hasDate = false,  QDate date = QDate::currentDate(),
           int uid = 0 /* empty */ );

    /* Copy c'tor

    **/
    OTodo(const OTodo & );

    /**
     *destructor
     */
    ~OTodo();

    /**
     * Is this event completed?
     */
    bool isCompleted() const;

    /**
     * Does this Event have a deadline
     */
    bool hasDueDate() const;

    /**
     * Does this Event has an alarm time ?
     */
    bool hasAlarmDateTime() const;

    /**
     * What is the priority?
     */
    int priority()const ;

    /**
     * progress as ushort 0, 20, 40, 60, 80 or 100%
     */
    ushort progress() const;

    /**
     * The due Date
     */
    QDate dueDate()const;

    /**
     * Alarm Date and Time
     */
    QDateTime alarmDateTime()const;

    /**
     * The description of the todo
     */
    QString description()const;

    /**
     * A small summary of the todo
     */
    QString summary() const;

    /**
     * @reimplemented
     * Return this todoevent in a RichText formatted QString
     */
    QString toRichText() const;

    /**
     * reimplementation
     */
    QString type()const;
    QString toShortText()const;
    QMap<QString, QString> toExtraMap()const;
    QString recordField(int id )const;

    /**
     * toMap puts all data into the map. int relates
     * to ToDoEvent RecordFields enum
     */
    QMap<int, QString> toMap()const;

    /**
     * Set if this Todo is completed
     */
    void setCompleted(bool completed );

    /**
     * set if this todo got an end data
     */
    void setHasDueDate( bool hasDate );

    /**
     * set if this todo has an alarm time and date
     */
    void setHasAlarmDateTime ( bool hasAlarm );

    /**
     * Set the priority of the Todo
     */
    void setPriority(int priority );

    /**
     * Set the progress.
     */
    void setProgress( ushort progress );

    /**
     * set the end date
     */
    void setDueDate( QDate date );

    /**
     * set the alarm time
     */
    void setAlarmDateTime ( const QDateTime& alarm );

    void setDescription(const QString& );
    void setSummary(const QString& );
    bool isOverdue();


    bool match( const QRegExp &r )const;

    bool operator<(const OTodo &toDoEvent )const;
    bool operator<=(const OTodo &toDoEvent )const;
    bool operator!=(const OTodo &toDoEvent )const;
    bool operator>(const OTodo &toDoEvent )const;
    bool operator>=(const OTodo &toDoEvent)const;
    bool operator==(const OTodo &toDoEvent )const;
    OTodo &operator=(const OTodo &toDoEvent );

 private:
    class OTodoPrivate;
    struct OTodoData;

    void deref();
    void changeOrModify();
    void copy( OTodoData* src, OTodoData* dest );
    OTodoPrivate *d;
    OTodoData *data;

};
inline bool OTodo::operator!=(const OTodo &toDoEvent )const {
    return !(*this == toDoEvent);
}


#endif
