#include "kmessagebox.h"
#include "klocale.h"

#include <qmessagebox.h>

void KMessageBox::sorry( QWidget *parent, 
         const QString &text,
         const QString &caption, bool )
{
  QString cap = caption;

  if (cap.isEmpty()) {
    cap = i18n("Sorry");
  }

  QMessageBox::warning( parent, cap, text );
}

int KMessageBox::warningYesNoCancel(QWidget *parent, const QString &text)
{
	int result = QMessageBox::warning(parent,i18n("Warning"),text,QMessageBox::Yes,
		QMessageBox::No, QMessageBox::Cancel);
	switch (result) {
			case QMessageBox::Yes: return Yes;
			case QMessageBox::No: return No;
			case QMessageBox::Cancel: return Cancel;
	}
}

int KMessageBox::questionYesNo(QWidget *parent,
                         const QString &text,
                         const QString &textYes,
                         const QString &textNo,
                         bool notify=true )
{
	int result =QMessageBox::warning(parent,i18n("Question"),text,textYes,textNo);
	if ( result == 0 ) return KMessageBox::Yes;
  	return KMessageBox::No;
}




int KMessageBox::warningContinueCancel( QWidget *parent, 
              const QString &text,
              const QString &caption,
              const QString &buttonContinue, 
              const QString &dontAskAgainName,
              bool notify )
{
  QString cap = caption;

  if (cap.isEmpty()) {
    cap = i18n("Warning");
  }

  int result = QMessageBox::warning( parent, cap, text, i18n("Ok"),
                                     i18n("Cancel") );

  if ( result == 0 ) return KMessageBox::Continue;
  return KMessageBox::Cancel;
}

void KMessageBox::error( QWidget *parent, 
         const QString &text, 
         const QString &caption, bool notify )
{
  QString cap = caption;

  if (cap.isEmpty()) {
    cap = i18n("Error");
  }

  QMessageBox::critical( parent, cap, text );
}

void KMessageBox::information( QWidget *parent, 
                          const QString &text, 
                          const QString &caption,
                          const QString &, 
                          bool )
{
  QString cap = caption;

  if (cap.isEmpty()) {
    cap = i18n("Information");
  }

  QMessageBox::information( parent, cap, text );
}
