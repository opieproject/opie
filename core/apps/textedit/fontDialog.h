/****************************************************************************
** Created: Sun Jan 27 11:02:59 2002 fileDialog.h
copyright 2002 by L.J. Potter ljp@llornkcor.com

** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef FONTDIALOG_H
#define FONTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpe/fontdatabase.h>
#include <qfont.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;
class QGroupBox;
class QMultiLineEdit;

class FontDialog : public QDialog//  QWidget
{ 
    Q_OBJECT

public:
    FontDialog( QWidget * parent = 0, const char* name = 0 , bool modal=FALSE, WFlags fl = 0 );
    ~FontDialog();

    QLabel *FontTextLabel4;
    QGroupBox *FamilyGroup, *GroupBox2;
    QListBox *familyListBox, *styleListBox, *sizeListBox;
    QMultiLineEdit *MultiLineEdit1;
    QPushButton *PushButtonOk,*PushButtonCancel;
    QStringList families;

    FontDatabase fdb;
    QString family, style, size;
    QFont selectedFont;
    bool changedFonts;
    void populateLists();
    void clearListBoxes();
    void changeText();
protected slots:

    void familyListBoxSlot(const QString &);
    void styleListBoxSlot(const QString &);
    void sizeListBoxSlot(const QString &); 

    
};

#endif // FONTDIALOG_H
