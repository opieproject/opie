#ifndef ONOTEEDIT_H
#define ONOTEEDIT_H

#include <qglobal.h>
#if QT_VERSION < 300
#include <qmultilineedit.h>
#else
#include <qtmultilineedit.h>
#define QMultiLineEdit QtMultiLineEdit
#endif

class ONoteEdit: public QMultiLineEdit {

  Q_OBJECT

 public:
  
  ONoteEdit (QWidget * parent=0, const char * name=0);

 protected:

  virtual void newLine ();
};

#endif
