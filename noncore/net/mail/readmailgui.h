#ifndef READMAILGUI_H
#define READMAILGUI_H

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtextbrowser.h>
#include <qaction.h>
#include <qstring.h>
#include <qdialog.h>


class ReadMailGui : public QDialog
{
   Q_OBJECT

public:
    ReadMailGui( QWidget* parent = 0, const char* name = 0, bool modal = TRUE, WFlags fl = 0 );
    ~ReadMailGui();

    void passMessage( const QString & message ); 

private:

    QMainWindow * m_mainWindow;
    QToolBar * m_toolBar;
    QMenuBar * m_menuBar;
    QTextBrowser * m_textWindow;
    QAction * m_reply;

};

#endif
