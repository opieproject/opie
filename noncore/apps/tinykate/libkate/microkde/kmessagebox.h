#ifndef MINIKDE_KMESSAGEBOX_H
#define MINIKDE_KMESSAGEBOX_H

#include <qstring.h>

class QWidget;

class KMessageBox
{
  public:
    enum { Ok = 1, Cancel = 2, Yes = 3, No = 4, Continue = 5 };

    static void sorry(QWidget *parent, 
                    const QString &text,
                    const QString &caption = QString::null, bool notify=true);

    static int warningContinueCancel(QWidget *parent, 
                         const QString &text,
                         const QString &caption = QString::null,
                         const QString &buttonContinue = QString::null, 
                         const QString &dontAskAgainName = QString::null,
                         bool notify=true );


    static int questionYesNo(QWidget *parent, 
                         const QString &text,
                         const QString &textYes,
                         const QString &textNo,
                         bool notify=true );

    static int warningYesNoCancel(QWidget *parent, const QString &text);


    static void error(QWidget *parent, 
                    const QString &text, 
                    const QString &caption = QString::null, bool notify=true);

    static void information(QWidget *parent, 
                          const QString &text, 
                          const QString &caption = QString::null,
                          const QString &dontShowAgainName = QString::null, 
                          bool notify=true);
};


#endif
