/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include <qdialog.h>
#include <qpixmap.h>

class InputMethods;
class QCString;
class QPushButton;
class QTranslator;
class AppLauncher;
class DocumentList;

class FirstUse : public QDialog
{
    Q_OBJECT
public:
    FirstUse(QWidget* parent=0, const char * name=0, WFlags=0);
    ~FirstUse();

    bool restartNeeded() const { return needRestart; }

private slots:
    void message(const QCString &, const QByteArray &);
    void calcMaxWindowRect();

    void nextDialog();
    void previousDialog();
    void terminated( int, const QString & );
    void newQcopChannel(const QString & );

private:
    void paintEvent( QPaintEvent * );
    void mouseReleaseEvent( QMouseEvent * );
    void keyPressEvent( QKeyEvent *e );

    void loadPixmaps();

    void reloadLanguages();
    void drawText(QPainter &p, const QString &text);
    void updateButtons();

private:
    QPixmap splash;
    QPixmap buttons;

    InputMethods *inputMethods;
    QPushButton *back;
    QPushButton *next;
    int controlHeight;

    QString lang;
    QTranslator *transApp;
    QTranslator *transLib;

    bool needCalibrate;
    DocumentList *docList;
    AppLauncher *appLauncher;
    QWidget *taskBar;

    int currApp;
    int waitForExit;
    bool waitingForLaunch;
    bool needRestart;

    QFont defaultFont;
};
