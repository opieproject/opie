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

#include <qwidget.h>
#include <qframe.h>
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
class QListBox;
class QListBoxItem;
class NotesControl : public QFrame {
    Q_OBJECT
public:
    NotesControl( QWidget *parent=0, const char *name=0 );
    void performGrab();

    QPixmap notes;
    QMultiLineEdit *view;
    QListBox *box;
    QPushButton *saveButton, *deleteButton, *newButton;
    QString FileNamePath;
    bool loaded, edited, doPopulate;
    void save();
    void populateBox();
    void load();
    
private:
      QTimer menuTimer;
      DocLnk *doc;
      QString oldDocName;
      void focusOutEvent( QFocusEvent * );
      void load(const QString&);
private slots:
    void slotBeamButton();
    void slotBeamFinished( Ir*);
    void slotDeleteButton();
    void slotSaveButton();
    void slotDeleteButtonClicked();
    void slotNewButton();
    void boxPressed(int, QListBoxItem *, const QPoint&);
    void showMenu();
    void loadDoc( const DocLnk &);
    void slotViewEdited();
    void slotBoxSelected(const QString &);
//    void keyReleaseEvent( QKeyEvent *);

};

class NotesApplet : public QWidget {
    Q_OBJECT
public:
    NotesApplet( QWidget *parent = 0, const char *name=0 );
    ~NotesApplet();
    NotesControl *vc; 
public slots:
private:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );

private:
    QPixmap notesPixmap;
private slots:

        
};

#endif // __NOTES_APPLET_H__

