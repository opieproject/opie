
#ifndef tododb_h
#define tododb_h

#include <qvaluelist.h>

#include <opie/todoevent.h>

class ToDoDB
{
 public:
    // if no argument is supplied pick the default book 
    ToDoDB(const QString &fileName = QString::null );
    ~ToDoDB();
    QValueList<ToDoEvent> effectiveToDos(const QDate &from,
					 const QDate &to,
					 bool includeNoDates = true);
    QValueList<ToDoEvent> effectiveToDos(const QDate &start, bool includeNoDates = true );
    QValueList<ToDoEvent> rawToDos(); // all events
    QValueList<ToDoEvent> overDue();

    void addEvent(const ToDoEvent &event );
    void editEvent(const ToDoEvent &editEvent );
    void removeEvent(const ToDoEvent &event);

    void reload();
    void setFileName(const QString & );
    QString fileName()const;
    bool save();

 private:
    class ToDoDBPrivate;
    ToDoDBPrivate *d;
    QString m_fileName;
    QValueList<ToDoEvent> m_todos;
    void load();
};


#endif
