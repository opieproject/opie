/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "listedit.h"
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qpe/resource.h>


// --- ListEdit ---------------------------------------------------------------
ListEdit::ListEdit( QWidget *parent, const char *sName )
    : QWidget(parent, sName), TableDef(sName)
{
    // get font height
    int fh = fontMetrics().height();

    // create layout
    QGridLayout *layout=new QGridLayout(this);
    layout->setSpacing( 2 );
	layout->setMargin( 4 );

    // type table
    _typeTable = new QListView( this );
    ColumnDef *def=first();
    while( def ) {
        _typeTable->addColumn( def->getName() );
        def=next();
    }
    connect( _typeTable, SIGNAL( clicked(QListViewItem *, const QPoint &, int) ), this, SLOT( slotClick(QListViewItem *, const QPoint &, int ) ) );
    layout->addMultiCellWidget(_typeTable, 0,4,0,4);
    _currentItem=NULL;

    // edit field
    _stack=new QWidgetStack( this );
    _stack->setMaximumHeight(fh+5);
    layout->addMultiCellWidget(_stack, 5,5,0,2);
    _typeEdit = new QLineEdit( _stack );
    _stack->raiseWidget(_typeEdit );
    connect( _typeEdit, SIGNAL( textChanged(const QString &) ), this, SLOT( slotEditChanged(const QString &) ) );

    // combo box
    _box=new QComboBox( _stack );
    connect( _box, SIGNAL( activated(const QString &) ), this, SLOT( slotActivated(const QString &) ) );


    // add button
    QPushButton *btn = new QPushButton( Resource::loadPixmap( "checkbook/add" ), tr( "Add" ), this );
	connect( btn, SIGNAL( clicked() ), this, SLOT( slotAdd() ) );
	layout->addWidget( btn, 5, 3 );

    // delete button
    btn = new QPushButton( Resource::loadPixmap( "trash" ), tr( "Delete" ), this );
	connect( btn, SIGNAL( clicked() ), this, SLOT( slotDel() ) );
	layout->addWidget( btn, 5, 4 );
}

// --- ~ListEdit --------------------------------------------------------------
ListEdit::~ListEdit()
{
}


// --- slotEditTypeChanged ----------------------------------------------------
void ListEdit::slotEditChanged(const QString &str)
{
    if( !_currentItem || _currentColumn<0 ) return;
    _currentItem->setText(_currentColumn, str);
}

// --- slotAddType ------------------------------------------------------------
void ListEdit::slotAdd()
{
    // construct new row
    QString args[8];
    ColumnDef *pCol=this->first();
    int i=0;
    while( pCol && i<8 ) {
        args[i++]=pCol->getNewValue();
        pCol=this->next();
    }
    _currentItem=new QListViewItem(_typeTable, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7] );

    // fix uniques
    fixTypes();

    // display col 0 of new value
    QPoint pnt;
    slotClick(_currentItem, pnt, 0);
    _typeTable->setSelected( _currentItem, true );

    // make it selected
    _typeEdit->setCursorPosition(0);
    _typeEdit->setSelection(0, _typeEdit->text().length() );
}

// --- slotDel -------------------------------------------------------------
void ListEdit::slotDel()
{
    if( !_currentItem ) return;
    delete _currentItem;
    _currentItem=NULL;
    _typeEdit->setText("");
    _stack->raiseWidget(_typeEdit);
}


// --- fixTypes ----------------------------------------------------------------
// Makes sure all entries have a unique name and empty entries are replaced
// by a generic string. The first version performs the operation on a particular
// column, whereas the 2nd does it for all unique columns.
class ColMap {
    public:
        ColMap(QString sValue, QListViewItem *pEntry) {
            _sValue=sValue;
            _pEntry=pEntry;
        }
        QString &getValue() { return(_sValue); }
        QListViewItem *getItem() { return(_pEntry); }

    protected:
        QString _sValue;
        QListViewItem *_pEntry;
};

class ColList : public QList<QString>
{
    public:
        ColList() : QList<QString>() { }

    protected:
        int compareItems(QCollection::Item, QCollection::Item);
};

int ColList::compareItems(QCollection::Item i1, QCollection::Item i2) {
    return( ((QString *)i1)->compare(*(QString *)i2) );
}

void ListEdit::fixTypes(int iColumn)
{
    // get column def
    ColumnDef *pDef=this->at(iColumn);

    // create map of entries
    if( !_typeTable->childCount() ) return;
    ColMap **colMap=new (ColMap *)[_typeTable->childCount()];
    QListViewItem *cur=_typeTable->firstChild();
    ColList lst;
    for(int i=0; i<_typeTable->childCount(); i++) {
        colMap[i]=new ColMap(cur->text(iColumn), cur);
        lst.append( &(colMap[i]->getValue()) );
        cur=cur->nextSibling();
    }

    // fix empty entries
    int i=0;
    for(QString *ptr=lst.first(); ptr; ptr=lst.next()) {
        *ptr=ptr->stripWhiteSpace();
        if( ptr->isEmpty() ) {
            i++;
            if( i==1 ) *ptr=pDef->getNewValue();
            else ptr->sprintf("%s %d", (const char *)pDef->getNewValue(), i);
        }
    }

    // fix dups
    lst.sort();
    QString repl;
    for(uint iCur=0; iCur<lst.count()-1; iCur++) {
        QString *current=lst.at(iCur);
        for(uint iNext=iCur+1; iNext<lst.count(); iNext++ ) {
            if( *current!=*lst.at(iNext) ) continue;
            for(int i=2; ; i++) {
                repl.sprintf("%s %d", (const char *)*current, i);
                bool bDup=false;
                uint iChk=iNext+1;
                while( iChk<lst.count() ) {
                    QString *chk=lst.at(iChk);
                    if( !chk->startsWith(*current) ) break;
                    if( *chk==repl ) {
                        bDup=true;
                        break;
                    }
                    iChk++;
                }
                if( !bDup ) {
                    *lst.at(iNext)=repl;
                    break;
                }
            }
        }
    }
    lst.sort();

    // copy back clean up col map
    for(int i=0; i<_typeTable->childCount(); i++) {
        colMap[i]->getItem()->setText(iColumn, colMap[i]->getValue());
        delete colMap[i];
    }
    delete colMap;
}

void ListEdit::fixTypes()
{
    int i;
    ColumnDef *pDef;
    for(pDef=this->first(), i=0; pDef; pDef=this->next(), i++) {
        if( pDef->hasFlag(ColumnDef::typeUnique) )
            fixTypes(i);
    }
    _typeTable->sort();
}


// --- storeInList ------------------------------------------------------------
void ListEdit::storeInList(QStringList &lst)
{
    // delete old content
    lst.clear();

    // add new one
    fixTypes();
    QListViewItem *itm=_typeTable->firstChild();
    while( itm ) {
        int i=0;
        QString sAdd;
        ColumnDef *pDef;
        for(pDef=this->first(), i=0; pDef; pDef=this->next(), i++) {
            if( i>=1 ) sAdd+=";";
            sAdd += itm->text(i);
        }
        lst.append( sAdd );
        itm=itm->nextSibling();
    }
}


// --- slotClicked ------------------------------------------------------------
void ListEdit::slotClick(QListViewItem *itm, const QPoint &pnt, int col)
{
    (void)pnt; // get rid of unused warning;

    // save values
    _currentItem=itm;
    _currentColumn=col;
    if( itm==NULL ) {
        _typeEdit->setText("");
        _stack->raiseWidget(_typeEdit);
        return;
    }

    // display value
    if( _currentColumn<0 ) _currentColumn=0;
    ColumnDef *pDef=this->at(_currentColumn);
    if( pDef->isType(ColumnDef::typeString) ) {
        _typeEdit->setText( _currentItem->text(_currentColumn) );
        _stack->raiseWidget(_typeEdit);
    } else if( pDef->isType(ColumnDef::typeList) ){
        _box->clear();
        _box->insertStringList( pDef->getValueList() );
        QStringList::Iterator itr;
        int i=0;
        for(itr=pDef->getValueList().begin(); itr!=pDef->getValueList().end(); itr++) {
            if( (*itr)==_currentItem->text(_currentColumn) ) {
                _box->setCurrentItem(i);
                i=-1;
                break;
            }
            i++;
        }
        if( i>=0 ) {
            _box->insertItem( _currentItem->text(_currentColumn) );
            _box->setCurrentItem(i);
        }
        _stack->raiseWidget(_box);
    } else {
        qDebug( "Unsupported column type for column %s", (const char *)pDef->getName() );
        _typeEdit->setText("");
        _stack->raiseWidget(_typeEdit);
    }
}


// --- addColumnDef -----------------------------------------------------------
void ListEdit::addColumnDef(ColumnDef *pDef)
{
    _typeTable->addColumn( pDef->getName() );
    _vColumns.append(pDef);
}

// --- addData ----------------------------------------------------------------
void ListEdit::addData(QStringList &lst)
{
    // run through list
    QStringList::Iterator itr;
    for(itr=lst.begin(); itr!=lst.end(); itr++) {
        QStringList split=QStringList::split(";", *itr, true);
        QStringList::Iterator entry;
        QString args[8];
        int i=0;
        for(entry=split.begin(); entry!=split.end() && i<8; entry++, i++) {
            args[i]= (*entry);
        }
        while(i<8) {
            args[i++]="";
        }
        new QListViewItem(_typeTable, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
    }
}

// --- slotActivated ----------------------------------------------------------
void ListEdit::slotActivated(const QString &str)
{
    if( _currentItem==NULL || _currentColumn<0 ) return;
    _currentItem->setText(_currentColumn, str);
}
