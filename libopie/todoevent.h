
#ifndef todoevent_h
#define todoevent_h

#include <qdatetime.h>

class ToDoEvent {
    friend class ToDoDB;
 public:
    enum Priority { VERYHIGH=1, HIGH, NORMAL, LOW, VERYLOW };
    ToDoEvent( bool completed = false, int priority = NORMAL, 
	       const QString &category = QString::null, 
	       const QString &description = QString::null , 
	       bool hasDate = false, QDate date = QDate::currentDate(), int uid = -1 );
    bool isCompleted() const;
    bool hasDate() const;
    int priority()const ;
    QString category()const;
    QDate date()const;
    QString description()const;

    int uid()const { return m_uid;};
    void setCompleted(bool completed );
    void setHasDate( bool hasDate );
    // if the category doesn't exist we will create it 
    void setCategory( const QString &category );
    void setPriority(int priority );
    void setDate( QDate date );
    void setDescription(const QString& );
    bool isOverdue();

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
    QString m_category;
    QString m_desc;
    int m_uid;
};


#endif
