/****************************************************************************
** copyright 2002 ljp ljp@llornkcor.com
** Created: Sat Feb 23 19:44:17 2002
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef FILEPERMISSIONS_H
#define FILEPERMISSIONS_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QLabel;
class QLineEdit;
class QString;

class filePermissions : public QDialog
{ 
    Q_OBJECT

public:
    filePermissions( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 , const QString & fileName ="", bool useRemote = FALSE);
    ~filePermissions();

    QLineEdit *LineEdit1, *ModeLine;
    QLabel *TextLabel1, *TextLabel4, *TextLabel4_2, *TextLabel4_3, *TextLabel3_2_2, *TextLabel3_2, *TextLabel3, *TextLabel5_2, *TextLabel5;
    QCheckBox*CheckBox1, *CheckBox1_3, *CheckBox1_2, *CheckBox1_4, *CheckBox1_5, *CheckBox1_6, *CheckBox1_7, *CheckBox1_8, *CheckBox1_8_2;
    QLineEdit*GroupLineEdit, *OwnerLineEdit;
    QString modeStr, file;
    int i_mode;
    uint moder;
    bool isRemote;
private slots:
    void ownReadCheck();
    void ownWriteCheck();
    void ownExeCheck(); 

    void grpReadCheck();
    void grpWriteCheck();
    void grpExeCheck(); 

    void wrldReadCheck();
    void wrldWriteCheck();
    void wrldExeCheck(); 
    void accept();
};

#endif // FILEPERMISSIONS_H
