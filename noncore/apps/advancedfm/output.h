/****************************************************************************
** Form interface generated from reading ui file 'outputEdit.ui'
**
** Created: Fri Apr 12 15:12:44 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef OUTPUT_H
#define OUTPUT_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QMultiLineEdit;

class Output : public QDialog
{ 
    Q_OBJECT

public:
    Output( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Output();

    QMultiLineEdit* OutputEdit;

protected:
    QGridLayout* OutputLayout;
protected slots:
    void saveOutput();
};

#endif // OUTPUT_H
