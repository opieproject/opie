
#ifndef todoevent_h
#define todoevent_h


#include <qarray.h>
#include <qmap.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qdatetime.h>

#include <qpe/recordfields.h>

class ToDoEvent   {
public:
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
    friend class ToDoDB;
 public:
    // priorities from Very low to very high
    enum TaskPriority { VERYHIGH=1, HIGH, NORMAL, LOW, VERYLOW };
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
    ToDoEvent( bool completed = false, int priority = NORMAL,
	       const QStringList &category = QStringList(),
	       const QString &summary = QString::null ,
               const QString &description = QString::null,
               ushort progress = 0,
	       bool hasDate = false, QDate date = QDate::currentDate(), int uid = -1 );

    /* Copy c'tor

    **/
    ToDoEvent(const ToDoEvent & );

    /**
     * Is this event completed?
     */
    bool isCompleted() const;

    /**
     * Does this Event have a deadline
     */
    bool hasDate() const;

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
     * All category numbers as QString in a List
     */
    QStringList allCategories()const;

    /**
     * Same as above but with QArray<int>
     */
    QArray<int> categories() const;

    /**
     * The end Date
     */
    QDate date()const;

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
     * Return this todoevent in a RichText formatted QString
     */
    QString richText() const;

    /**
     * Returns the UID of the Todo
     */
    int uid()const { return m_uid;};


    QString extra(const  QString& )const;

    QMap<QString, QString> extras()const { return m_extra; };
    /**
     * returns a list of apps which have related items
     */
    QStringList relatedApps()const;

    /**
     * returns all relations for one app
     */
    QArray<int> relations( const QString& app )const;
    /**
     * Set if this Todo is completed
     */
    void setCompleted(bool completed );

    /**
     * set if this todo got an end data
     */
    void setHasDate( bool hasDate );

    /**
     * set if this todo has an alarm time and date
     */
    void setHasAlarmDateTime ( bool hasAlarm );

    // if the category doesn't exist we will create it
    // this sets the the Category after this call category will be the only category
    void setCategory( const QString &category );
    // adds a category to the Categories of this event
    void insertCategory(const QString &category );

    /**
     * Removes this event from all categories
     */
    void clearCategories();

    /**
     * This todo belongs to xxx categories
     */
    void setCategories(const QStringList& );

    /**
     * Set the priority of the Todo
     */
    void setPriority(int priority );

    /**
     * Set the progress.
     */
    void setProgress( ushort progress );

    /**
     * add related function it replaces too ;)
     */
    void addRelated( const QString& app, int id );

    /**
     * add related
     */
    void addRelated( const QString& app, QArray<int> ids );

    /**
     * clear relations for one app
     */
    void clearRelated(const QString& app);

    /**
     * set the end date
     */
    void setDate( QDate date );

    /**
     * set the alarm time
     */
    void setAlarmDateTime ( const QDateTime& alarm );

    void setDescription(const QString& );
    void setSummary(const QString& );
    void setExtra( const QString&,  const QString& );
    bool isOverdue();

    bool match( const QRegExp &r )const;

    void setUid(int id) {m_uid = id; };
    bool operator<(const ToDoEvent &toDoEvent )const;
    bool operator<=(const ToDoEvent &toDoEvent )const;
    bool operator!=(const ToDoEvent &toDoEvent )const { return !(*this == toDoEvent); };
    bool operator>(const ToDoEvent &toDoEvent )const;
    bool operator>=(const ToDoEvent &toDoEvent)const;
    bool operator==(const ToDoEvent &toDoEvent )const;
    ToDoEvent &operator=(const ToDoEvent &toDoEvent );

 private:
    class ToDoEventPrivate;
    ToDoEventPrivate *d;
    QDate m_date;
    bool m_isCompleted:1;
    bool m_hasDate:1;
    int m_priority;
    QStringList m_category;
    QString m_desc;
    QString m_sum;
    QMap<QString, QString> m_extra;
    QMap<QString, QArray<int> > m_relations;
    int m_uid;
    ushort m_prog;

    /** Alarm Time stuff */
    bool m_hasAlarmDateTime;
    QDateTime m_alarmDateTime;
};


#endif
