#include "noteentryimpl.h"
#include "qstring.h"
#include "qmultilinedit.h"
#include "qlabel.h"
#include "onoteedit.h"

NoteEntry::NoteEntry(const QString &title, const QString &noteStr,
		     QWidget* parent, const char* name, bool modal,
		     WFlags fl) :
  NoteEntryBase(parent, name, modal, fl) {


  eventLabel->setText(title);
  note->setText(noteStr);
  int l=note->length();
  note->setCursorPosition(l,l,false);
}
