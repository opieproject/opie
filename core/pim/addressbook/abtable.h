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

#ifndef ABTABLE_H
#define ABTABLE_H

#include <qpe/categories.h>
#include <qpe/contact.h>

#include <qmap.h>
#include <qtable.h>
#include <qstringlist.h>
#include <qcombobox.h>

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

class AbPickItem : public QTableItem
{
public:
    AbPickItem( QTable *t );

    QWidget *createEditor() const;
    void setContentFromEditor( QWidget *w );

private:
    QGuardedPtr<QComboBox> cb;
};

class AbTable : public QTable
{
    Q_OBJECT

public:
    AbTable( const QValueList<int> *ordered, QWidget *parent, const char *name=0 );
    ~AbTable();
    // NEW
    void addEntry( const Contact &newContact );
    Contact currentEntry();
    void replaceCurrentEntry( const Contact &newContact );

    void init();

    void deleteCurrentEntry();
    void clear();
    void clearFindRow() { currFindRow = -2; }
    void loadFields();
    void refresh();
    bool save( const QString &fn );
    void load( const QString &fn );

    // addresspicker mode
    void setChoiceNames( const QStringList& list);
    QStringList choiceNames() const;
    void setChoiceSelection(int index, const QStringList& list);
    QStringList choiceSelection(int index) const;
    void setShowCategory( const QString &c );
    void setShowByLetter( char c );
    QString showCategory() const;
    QStringList categories();
    void resizeRows( int size );

    void show();
    void setPaintingEnabled( bool e );

public slots:
    void slotDoFind( const QString &str, bool caseSensitive, bool backwards,
                     int category );
signals:
    void empty( bool );
    void details();
    void signalNotFound();
    void signalWrapAround();

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
    void loadFile( const QString &strFile, bool journalFile );
    void fitColumns();
    void resort();
    void updateJournal( const Contact &contact, Contact::journal_action action,
			int row = -1 );
    void insertIntoTable( const Contact &contact, int row );
    void internalAddEntries( QList<Contact> &list );
    QString findContactName( const Contact &entry );
    QString findContactContact( const Contact &entry );
    void journalFreeReplace( const Contact &cnt, int row );
    void journalFreeRemove( int row );
    void realignTable( int );
    void updateVisible();
    int lastSortCol;
    bool asc;
    char showChar;
    QMap<AbTableItem*, Contact> contactList;
    const QValueList<int> *intFields;
    int currFindRow;
    QString showCat;
    QStringList choicenames;
    bool enablePainting;
    Categories mCat;
};
#endif // ABTABLE_H
