#ifndef noteentryimpl_h
#define noteentryimpl_h

#include "noteentry.h"

class QString;

class NoteEntry : public NoteEntryBase 
{
    Q_OBJECT

public:
    NoteEntry(const QString &title, const QString &noteStr,
	      QWidget* parent = 0, const char* name = 0, 
	      bool modal=TRUE, WFlags fl=0);
};

#endif
