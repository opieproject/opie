#ifndef ONOTEEDIT_H
#define ONOTEEDIT_H

#include <qmultilineedit.h>

class ONoteEdit: public QMultiLineEdit {

  Q_OBJECT

 public:
  
  ONoteEdit (QWidget * parent=0, const char * name=0);

 protected:

  virtual void newLine ();
};

#endif
