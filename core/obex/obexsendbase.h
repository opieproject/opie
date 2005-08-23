/****************************************************************************
** Form interface generated from reading ui file 'obexsendbase.ui'
**
** Created: Fri Aug 5 00:14:51 2005
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef OBEXSENDBASE_H
#define OBEXSENDBASE_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListView;
class QListViewItem;
class QPushButton;

class obexSendBase : public QWidget
{ 
    Q_OBJECT

public:
    obexSendBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~obexSendBase();

    QLabel* sendLabel;
    QLabel* fileToSend;
    QLabel* irdaLabel;
    QLabel* irdaStatus;
    QLabel* btLabel;
    QLabel* btStatus;
    QListView* receiverList;
    QPushButton* scanButton;
    QPushButton* sendButton;
    QPushButton* doneButton;

protected slots:
    virtual void scan_for_receivers();
    virtual void send_to_receivers();
    virtual void toggle_receiver(QListViewItem *);
    virtual void userDone();

protected:
    QVBoxLayout* obexSendBaseLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout4;
    QHBoxLayout* Layout3;
};

#endif // OBEXSENDBASE_H
