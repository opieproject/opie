
#ifndef XMLOPIETODO_RESOURCE_H
#define XMLOPIETODO_RESOURCE_H

#include <qvaluelist.h>

#include <opie/todoevent.h>
#include <opie/todoresource.h>

class FileToDoResource : public ToDoResource {
public:
    FileToDoResource() {};
    virtual ~FileToDoResource() {};
    bool save( const QString& name,
               const QValueList<ToDoEvent>& m_todos );

    QValueList<ToDoEvent> load( const QString& name );



};

#endif
