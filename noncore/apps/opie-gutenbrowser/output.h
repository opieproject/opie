/****************************************************************************
** Form interface generated from reading ui file 'outputEdit.ui'
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
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
};

#endif // OUTPUT_H
