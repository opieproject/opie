/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef ABEDITOR_H
#define ABEDITOR_H

#include <qpe/contact.h>

#include <qdialog.h>
#include <qlist.h>
#include <qmap.h>
#include <qstringlist.h>

class QScrollView;
class QMultiLineEdit;
class QLineEdit;
class QLabel;
class QComboBox;
class CategorySelect;

class AbEditor : public QDialog
{
    Q_OBJECT
public:
    AbEditor( const OContact &entry, const QValueList<int> *newOrdedValues,
	      QStringList *slNewOrdered,
	      QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~AbEditor();
    void loadFields();
    void setNameFocus();
    OContact entry() const { return ent; }

public slots:
    void slotNote();
    void setEntry( const OContact &entry );

protected slots:
    void accept();

private:
    void init();
    void initMap();
    void saveEntry();
    bool isEmpty();

private:
    QDialog *dlgNote;
    QLabel *lblNote;
    QMultiLineEdit *txtNote;
    OContact ent;
    QScrollView *svPage;
    QLineEdit *firstEdit;
    QLineEdit *lastEdit;
    QLineEdit *middleEdit;
    QComboBox *genderCombo;
    QList<QLineEdit> listValue;
    QList<QLabel> listName;
    const QValueList<int> *orderedValues;
    QStringList *slOrdered;
    CategorySelect *cmbCat;
};

#endif
