/****************************************************************************
** Form interface generated from reading ui file 'qchecknamebase.ui'
**
** Created: Wed Sep 18 09:40:11 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef QCHECKNAMEBASE_H
#define QCHECKNAMEBASE_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QPushButton;
class QRestrictedLine;

class QCheckNameBase : public QDialog
{ 
    Q_OBJECT

public:
    QCheckNameBase( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~QCheckNameBase();

    QLabel* TextLabel3;
    QLabel* TextLabel4;
    QPushButton* cmdDone;
    QRestrictedLine* leText;

protected:
    bool event( QEvent* );
};

#endif // QCHECKNAMEBASE_H
