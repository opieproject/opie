#ifndef QSMB_H
#define QSMB_H

#include "qsmbbase.h"

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>

#include <qlistview.h>
#include <pthread.h>

class Qsmb : public FormQPESMBBase
{ 
   Q_OBJECT

public:
   static QString appName() { return QString::fromLatin1("opie-smb"); }
   Qsmb( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
   ~Qsmb();
   void scan();
   void DoIt();

private:
   QListViewItem *top_element;
   QComboBox *hosts;
   pthread_t tpid;
   bool scanning;
   bool isMounted(const QString &);
   QString getMount(const QString &);

public slots:
   void clear();
	 void scanClicked();
   void hostSelected(int);
   void DoItClicked();
   void umountIt();
   QString out;
   bool runCommand(const QStringList &);

};
void* runit(void *arg);
void* runitm(void *arg);

#endif // QSMB_H
