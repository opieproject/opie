/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
** Copyright (c) 2002 Stefan Eilers (eilers.stefan@epost.de)
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

#ifndef ABTABLE_H
#define ABTABLE_H

#include <opie2/opimcontact.h>
#include <opie2/ocontactaccess.h>

#include <qpe/categories.h>

#include <qmap.h>
#include <qtable.h>
#include <qstringlist.h>
#include <qcombobox.h>
#include <qpixmap.h>

#if 0
class AbTableItem : public QTableItem
{
public:
    AbTableItem( QTable *t, EditType et, const QString &s,
		 const QString &secondSortKey);
    QString entryKey() const;
    void setEntryKey( const QString & k );
    virtual int alignment() const;
    virtual QString key() const;
    void setItem( const QString &txt, const QString &secondKey );

private:
    QString sortKey;
};

#endif

// This is a simple container, storing all contact
// information
class ContactItem
{
 public:
	QPixmap icon;
	QString value;
};

#if 0
class AbPickItem : public QTableItem
{
public:
    AbPickItem( QTable *t );

    QWidget *createEditor() const;
    void setContentFromEditor( QWidget *w );

private:
    QGuardedPtr<QComboBox> cb;
};

#endif

class AbTable : public QTable
{
    Q_OBJECT

public:
    AbTable( const QValueList<int> ordered, QWidget *parent, const char *name=0 );
    ~AbTable();

    // Set the contacts shown in the table
    void setContacts( const Opie::OPimContactAccess::List& viewList );

    // Set the list for primary contacts
    void setOrderedList( const QValueList<int> ordered );

    // Selects a contact of a specific UID
    bool selectContact( int UID );

    // Get the current selected entry
    Opie::OPimContact currentEntry();

    // Get the UID of the current selected Entry
    int currentEntry_UID();

    // QString findContactName( const Opie::OPimContact &entry );

    void init();
    void clear();
    void refresh();

    void show();
    void setPaintingEnabled( bool e );
    void viewportPaintEvent( QPaintEvent* e);
    void paintCell(QPainter* p, int row, int col,  const QRect&, bool );

    // addresspicker mode (What's that ? se)
    // void setChoiceNames( const QStringList& list);
    // QStringList choiceNames() const;
    void setChoiceSelection( const QValueList<int>& list );
    QStringList choiceSelection(int index) const;

signals:
    void signalSwitch();
    void signalEditor();
    void signalKeyDown();
    void signalKeyUp();

protected:
    virtual void keyPressEvent( QKeyEvent *e );

//     int rowHeight( int ) const;
//     int rowPos( int row ) const;
//     virtual int rowAt( int pos ) const;


protected slots:
    void moveTo( char );
    virtual void columnClicked( int col );
    void itemClicked(int,int col);
    void rowHeightChanged( int row );

private:
    // void insertIntoTable( const Opie::OPimContact &cnt, int row );
    ContactItem findContactContact( const Opie::OPimContact &entry, int row );
    void fitColumns();
    void resizeRows();
    void realignTable();
    void resort();
    void updateVisible();

    int lastSortCol;
    bool asc;
    // QMap<AbTableItem*, Opie::OPimContact> contactList;
    QValueList<int> intFields;
    QStringList choicenames;
    bool enablePainting;
    bool columnVisible;
    int countNested;

    Opie::OPimContactAccess::List m_viewList;

};
#endif // ABTABLE_H
