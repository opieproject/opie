#include "onoteedit.h"

ONoteEdit::ONoteEdit (QWidget * parent, const char * name) :
  QMultiLineEdit(parent,name) {
  setDefaultTabStop(2); 
  setWordWrap(WidgetWidth);
}

// Copy indentation from previous line
void ONoteEdit::newLine() {
  int l,c;
  getCursorPosition(&l,&c);
  QString s=textLine(l);

  insert("\n");

  int i=0;
  while (s[i]==' ' || s[i]=='\t') insert(QString(s[i])), i++;
}
