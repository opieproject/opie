/****************************************************************************
** Form interface generated from reading ui file 'todaybase.ui'
**
** Created: Wed Feb 13 19:36:45 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef TODAYBASE_H
#define TODAYBASE_H

#include <qvariant.h>
#include <qwidget.h>
#include <qscrollview.h>
#include <qsplitter.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;

class TodayBase : public QWidget
{ 
    Q_OBJECT

public:
    TodayBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TodayBase();

    //QScrollView* Frame4;
    QFrame* Frame4;
    QPushButton* DatesButton;
    QLabel* DatesField;
    QLabel* Frame;
    QFrame* MailFrame;
    QLabel* TextLabel1;
    QFrame* Frame15;
    QLabel* TodoField;
    QLabel* MailField;
    QPushButton* PushButton1;
    QPushButton* TodoButton;
    QPushButton* MailButton;
    QPushButton* getridoffuckingstrippeldlinesbutton;

protected:


};

#endif // TODAYBASE_H
