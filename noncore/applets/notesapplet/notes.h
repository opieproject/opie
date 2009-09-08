/**********************************************************************
** Copyright (C) 2002 L.J. Potter ljp@llornkcor.com
**  All rights reserved.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef __NOTES_APPLET_H__
#define __NOTES_APPLET_H__

#include "notesmanager.h"

#include <opie2/omemoaccess.h>

#include <qwidget.h>
#include <qlistbox.h>
#include <qvbox.h>
#include <qpixmap.h>
#include <qguardedptr.h>
#include <qtimer.h>
#include <qpe/filemanager.h>
#include <qpe/ir.h>
#include <qstring.h>

class QComboBox;
class QCheckBox;
class QSpinBox;
class QPushButton;
class QMultiLineEdit;

namespace Opie {
namespace Notes {

class MemoListItem : public QListBoxText
{
public:
    MemoListItem ( QListBox *listbox, const QString &text, int uid );
    int uid();
private:
    int m_uid;
};


class NotesControl : public QVBox {
    Q_OBJECT
public:
    NotesControl( QWidget *parent=0, const char *name=0 );

    QMultiLineEdit *m_editArea;
    QListBox *m_notesList;
    QPushButton *saveButton, *deleteButton, *newButton;
    bool m_loaded, m_edited, m_doPopulate;
    bool showMax;
    void save();
    void refreshList();
    void load();

private:
    QTimer menuTimer;
    DocLnk *doc;
    QString oldDocName;
    NotesManager m_manager;
    int m_selected;
    OPimMemo m_currentMemo;

    void focusOutEvent( QFocusEvent * );
    void populateList( OPimMemoAccess::List &list );
    int selectedMemoUid();

private slots:
    void slotShowMax();
    void slotBeamButton();
    void slotBeamFinished( Ir*);
    void slotDeleteButton();
    void slotSaveButton();
    void slotNewButton();
    void boxPressed(int, QListBoxItem *, const QPoint&);
    void showMenu();
    void slotViewEdited();
    void slotBoxSelected(const QString &);

};

class NotesApplet : public QWidget {
    Q_OBJECT
public:
    NotesApplet( QWidget *parent = 0, const char *name=0 );
    ~NotesApplet();
    static int position();
    NotesControl *vc;
public slots:
private:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );

private:
    QPixmap notesPixmap;
private slots:


};

}
}

#endif // __NOTES_APPLET_H__

