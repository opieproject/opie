/****************************************************************************
** Form interface generated from reading ui file 'commandeditwidget.ui'
**
** Created: Sat Feb 2 11:08:25 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef FORM1_H
#define FORM1_H

#include <qvariant.h>
#include <qwidget.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class PlayListSelection;

class Form1 : public QWidget
{ 
    Q_OBJECT

public:
    Form1( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Form1();

    PlayListSelection* MyCustomWidget1;

protected:
    QGridLayout* Form1Layout;
};

#endif // FORM1_H
