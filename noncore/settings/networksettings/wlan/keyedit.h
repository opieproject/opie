#ifndef KEYEDIT_H
#define KEYEDIT_H

#include <qlineedit.h>

class KeyEdit : public QLineEdit { 
  Q_OBJECT

public:
  KeyEdit(QWidget* parent = 0, const char* name = 0);
  ~KeyEdit();

protected:
  void focusInEvent(QFocusEvent *event);
  void focusOutEvent(QFocusEvent *event);

private:
};

#endif
 
