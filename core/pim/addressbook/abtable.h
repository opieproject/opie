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
#include <opie/ocontact.h>

#include <qmap.h>
#include <qtable.h>
#include <qstringlist.h>
#include <qcombobox.h>

#include <opie/ocontactaccess.h>

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
    void addEntry( const OContact &newContact );
    OContact currentEntry();
    void replaceCurrentEntry( const OContact &newContact );

    void init();

    void deleteCurrentEntry();
    void clear();
    void clearFindRow() { currFindRow = -1; }
    void loadFields();
    void refresh();
    bool save();
    void load();
    void reload();

    // addresspicker mode
    void setChoiceNames( const QStringList& list);
    QStringList choiceNames() const;
    void setChoiceSelection(int index, const QStringList& list);
    QStringList choiceSelection(int index) const;
    void setShowCategory( const QString &b, const QString &c );
    void setShowByLetter( char c );
    QString showCategory() const;
    QStringList categories();

    void resizeRows();

    void show();
    void setPaintingEnabled( bool e );

    QString showBook() const;

    void inSearch()  { m_inSearch = true; }
    void offSearch() { m_inSearch = false; }

public slots:
    void slotDoFind( const QString &str, bool caseSensitive, bool useRegExp, bool backwards,
                     QString category = QString::null );
signals:
    void empty( bool );
    void details();
    void signalNotFound();
    void signalWrapAround();
    void signalSearchBackward(); // Signalled if backward search is requested
    void signalSearchNext();    // Singalled if forward search is requested

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
    void updateJournal( const OContact &contact, OContact::journal_action action,
			int row = -1 );
    void insertIntoTable( const OContact &contact, int row );
    QString findContactName( const OContact &entry );
    QString findContactContact( const OContact &entry, int row );
    void journalFreeReplace( const OContact &cnt, int row );
    void journalFreeRemove( int row );
    void realignTable( int );
    void updateVisible();
    int lastSortCol;
    bool asc;
    char showChar;
    QMap<AbTableItem*, OContact> contactList;
    const QValueList<int> *intFields;
    int currFindRow;
    QString showCat;
    QStringList choicenames;
    bool enablePainting;
    Categories mCat;

    QString showBk;
    bool columnVisible;

    bool m_inSearch;

    OContactAccess m_contactdb;

};
#endif // ABTABLE_H
