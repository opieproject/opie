

#ifndef opietodoresource_h
#define opietodoresource_h

class ToDoEvent;
class ToDoResource {
 public:
  ToDoResource( ) {};
  virtual QValueList<ToDoEvent> load(const QString &file ) = 0;
  virtual bool save( const QString &file, const QValueList<ToDoEvent> & ) = 0;
};

#endif
