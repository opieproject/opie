/****************************************************************************
** Form interface generated from reading ui file 'shutdown.ui'
**
** Created: Mon Aug 25 18:12:35 2003
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
class QButtonGroup;
class QLabel;
class QProgressBar;
class QPushButton;

class Shutdown : public QWidget
{ 
    Q_OBJECT

public:
    Shutdown( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Shutdown();

    QButtonGroup* ButtonGroup1;
    QPushButton* quit;
    QPushButton* reboot;
    QPushButton* restart;
    QPushButton* shutdown;
    QLabel* info;
    QProgressBar* progressBar;
    QPushButton* cancel;

protected:
    QVBoxLayout* Form1Layout;
    QGridLayout* ButtonGroup1Layout;
};

#endif // FORM1_H
