/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#ifndef TODOTABLE_H
#define TODOTABLE_H

#include <qpe/categories.h>
#include <qpe/stringutil.h>
//#include <qpe/task.h>
#include <opie/todoevent.h>

#include <qtable.h>
#include <qmap.h>
#include <qguardedptr.h>

class Node;
class QComboBox;
class QTimer;

class CheckItem : public QTableItem
{
public:
    CheckItem( QTable *t, const QString &sortkey );

    void setChecked( bool b );
    void toggle();
    bool isChecked() const;
    void setKey( const QString &key ) { sortKey = key; }
    QString key() const;

    void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );

private:
    bool checked;
    QString sortKey;
};

class ComboItem : public QTableItem
{
public:
    ComboItem( QTable *t, EditType et );
    QWidget *createEditor() const;
    void setContentFromEditor( QWidget *w );
    void setText( const QString &s );
    int alignment() const { return Qt::AlignCenter; }

    QString text() const;

private:
    QGuardedPtr<QComboBox> cb;

};

class TodoTextItem : public QTableItem
{
public:
    TodoTextItem( QTable *t, const QString & str ) 
	:QTableItem( t, QTableItem::Never, str ) {}

    QString key () const { return Qtopia::buildSortKey( text() ); }
};

class DueTextItem : public QTableItem
{
 public:
  DueTextItem( QTable *t, ToDoEvent *ev );
  QString key() const;
  void setToDoEvent( const ToDoEvent *ev );
  void paint( QPainter *p, const QColorGroup &cg, const QRect &cr, bool selected );
 private:
  int m_off;
  bool m_hasDate:1;
  bool m_completed:1;

};


enum journal_action { ACTION_ADD, ACTION_REMOVE, ACTION_REPLACE };

class TodoTable : public QTable
{
    Q_OBJECT

public:
    TodoTable( QWidget *parent = 0, const char * name = 0 );
    void addEntry( const ToDoEvent &todo );
    void clearFindRow() { currFindRow = -2; }

    ToDoEvent currentEntry() const;
    void replaceCurrentEntry( const ToDoEvent &todo, bool fromTableItem = false );

    QStringList categories();

    void setShowCompleted( bool sc ) { showComp = sc; updateVisible(); }
    bool showCompleted() const { return showComp; }

    void setShowDeadline (bool sd) {showDeadl = sd; updateVisible();}
    bool showDeadline() const { return showDeadl;}

    void setShowCategory( const QString &c ) { showCat = c; updateVisible(); }
    const QString &showCategory() const { return showCat; }
    int showCategoryId() const;

    bool save( const QString &fn );
    void load( const QString &fn );
    void clear();
    void removeCurrentEntry();

    void setPaintingEnabled( bool e );

    virtual void sortColumn( int col, bool ascending, bool /*wholeRows*/ );

//     int rowHeight( int ) const;
//     int rowPos( int row ) const;
//     virtual int rowAt( int pos ) const;

signals:
    void signalEdit();
    void signalDoneChanged( bool b );
    void signalPriorityChanged( int i );
    void signalShowMenu( const QPoint & );
    void signalNotFound();
    void signalWrapAround();

protected:
    void keyPressEvent( QKeyEvent *e );

private:
    void updateVisible();
    void viewportPaintEvent( QPaintEvent * );
    void internalAddEntries( QList<ToDoEvent> &list);
    inline void insertIntoTable( ToDoEvent *todo, int row );
    void updateJournal( const ToDoEvent &todo, journal_action action, int row = -1);
    void mergeJournal();
    void journalFreeReplaceEntry( const ToDoEvent &todo, int row );
    void journalFreeRemoveEntry( int row );
    inline void realignTable( int row );
    void loadFile( const QString &strFile, bool fromJournal = false );

private slots:
    void slotClicked( int row, int col, int button, const QPoint &pos );
    void slotPressed( int row, int col, int button, const QPoint &pos );
    void slotCheckPriority(int row, int col );
    void slotCurrentChanged(int row, int col );
    void slotDoFind( const QString &findString, bool caseSensetive,
		     bool backwards, int category );
    void slotShowMenu();
    void rowHeightChanged( int row );

private:
    friend class TodoWindow;

    QMap<CheckItem*, ToDoEvent *> todoList;
    QStringList categoryList;
    bool showComp;
    QString showCat;
    QTimer *menuTimer;
    bool enablePainting;
    Categories mCat;
    int currFindRow;
    bool showDeadl:1;
};


inline void TodoTable::insertIntoTable( ToDoEvent *todo, int row )
{
    QString sortKey = (char) ((todo->isCompleted() ? 'a' : 'A') 
			      + todo->priority() ) 
		      + Qtopia::buildSortKey( todo->description() );
    CheckItem *chk = new CheckItem( this, sortKey );
    chk->setChecked( todo->isCompleted() );
    ComboItem *cmb = new ComboItem( this, QTableItem::WhenCurrent );
    cmb->setText( QString::number( todo->priority() ) );
    QTableItem *ti = new TodoTextItem( this, todo->description().left(40).simplifyWhiteSpace() );
    ti->setReplaceable( false );

    DueTextItem *due = new DueTextItem(this, todo );
    setItem( row, 3, due);
    
    setItem( row, 0, chk );
    setItem( row, 1, cmb );
    setItem( row, 2, ti );


    todoList.insert( chk, todo );
}

inline void TodoTable::realignTable( int row )
{
  QTableItem *ti1,
    *ti2,
    *ti3,
    *ti4;
    int totalRows = numRows();
    for ( int curr = row; curr < totalRows - 1; curr++ ) {
	// this is bad, we must take the item out and then
	// set it. In the end, it behaves no worse (time wise)
	// then the old way of saving the entries to file, clearing
	// the table re-reading in the file and resetting the table
	ti1 = item( curr + 1, 0 );
	ti2 = item( curr + 1, 1 );
	ti3 = item( curr + 1, 2 );
	ti4 = item( curr + 1, 3 );
	takeItem( ti1 );
	takeItem( ti2 );
	takeItem( ti3 );
	takeItem( ti4 );
	setItem( curr, 0, ti1 );
	setItem( curr, 1, ti2 );
	setItem( curr, 2, ti3 );
	setItem( curr, 3, ti4 );
    }
    setNumRows( totalRows - 1 );
}

#endif
