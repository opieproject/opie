
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


class OPimState;
class ORecur;
class OPimMaintainer;
class OPimNotifyManager;
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
        State,
        Recurrence,
        Alarms,
        Reminders,
        Notifiers,
        Maintainer,
        StartDate,
        CompletedDate
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

    /** Copy c'tor
     *
     */
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
    bool hasStartDate()const;
    bool hasCompletedDate()const;

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
     * When did it start?
     */
    QDate startDate()const;

    /**
     * When was it completed?
     */
    QDate completedDate()const;

    /**
     * What is the state of this OTodo?
     */
    OPimState state()const;

    /**
     * the recurrance of this
     */
    ORecur recurrence()const;

    /**
     * the Maintainer of this OTodo
     */
    OPimMaintainer maintainer()const;

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

    /*
     * check if the sharing is still fine!! -zecke
     */
    /**
     * return a reference to our notifiers...
     */
    OPimNotifyManager &notifiers();

    /**
     * reimplementations
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
    // FIXME we do not have these for start, completed
    // cause we'll use the isNull() of QDate for figuring
    // out if it's has a date...
    // decide what to do here? -zecke

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
    void setDueDate( const QDate& date );

    /**
     * set the start date
     */
    void setStartDate( const QDate& date );

    /**
     * set the completed date
     */
    void setCompletedDate( const QDate& date );

    void setRecurrence( const ORecur& );
    /**
     * set the alarm time
     */
    void setAlarmDateTime ( const QDateTime& alarm );

    void setDescription(const QString& );
    void setSummary(const QString& );

    /**
     * set the state of a Todo
     * @param state State what the todo should take
     */
    void setState( const OPimState& state);

    /**
     * set the Maintainer Mode
     */
    void setMaintainer( const OPimMaintainer& );

    bool isOverdue();


    bool match( const QRegExp &r )const;

    bool operator<(const OTodo &toDoEvent )const;
    bool operator<=(const OTodo &toDoEvent )const;
    bool operator!=(const OTodo &toDoEvent )const;
    bool operator>(const OTodo &toDoEvent )const;
    bool operator>=(const OTodo &toDoEvent)const;
    bool operator==(const OTodo &toDoEvent )const;
    OTodo &operator=(const OTodo &toDoEvent );

    static int rtti();

 private:
    class OTodoPrivate;
    struct OTodoData;

    void deref();
    inline void changeOrModify();
    void copy( OTodoData* src, OTodoData* dest );
    OTodoPrivate *d;
    OTodoData *data;

};
inline bool OTodo::operator!=(const OTodo &toDoEvent )const {
    return !(*this == toDoEvent);
}


#endif
