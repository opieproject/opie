/****************************************************************************
** Created: Sun Jan 27 11:02:59 2002
copyright 2002 by L.J. Potter ljp@llornkcor.com
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
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
class QComboBox;

class FontDialog : public /*QDialog*/  QWidget
{ 
    Q_OBJECT

public:
    FontDialog( QWidget * parent = 0, const char* name = 0 /*, bool modal=FALSE, WFlags fl = 0 */);
    ~FontDialog();

    QLabel *FontTextLabel4;
    QGroupBox *FamilyGroup, *GroupBox2;
    QListBox *familyListBox, *styleListBox, *sizeListBox;
    QMultiLineEdit *MultiLineEdit1;
    QPushButton *PushButtonOk,*PushButtonCancel;
    QStringList families;
    QComboBox *sizeComboBox;

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
    void sizeComboBoxSlot(const QString &); 
//    void sizeListBoxSlot(const QString &); 

    
};

#endif // FONTDIALOG_H
