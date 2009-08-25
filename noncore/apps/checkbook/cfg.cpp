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

#include <stdio.h>

#include <qwidget.h>
#include <qpe/config.h>

#include "cfg.h"

// --- Cfg --------------------------------------------------------------------
Cfg::Cfg()
    : m_currencySymbol("$")
    , m_useSmallFont(true)
    , m_showLocks(false)
    , m_showBalances(false)
    , m_openLastBook(false)
    , m_showLastTab(false)
    , m_Dirty(false)
    , m_SavePayees(false)
    , m_LastBook()
    , m_Categories(new CategoryList())
{
}

Cfg::~Cfg()
{
    delete m_Categories;
}

// --- readStringList ---------------------------------------------------------
// Reads the entries for the control from a configuration file and returns
// them in a StringList. Later this list can be used to create the control. It
// is assumed, that the group is already set. Key is used to enumerate the
// entries.
void Cfg::readStringList(Config &cfg, const char *sKey, QStringList &lst)
{
    QString sEntry;
    int iCount;

    // read count of elements
    sEntry.sprintf("%s_Count", sKey);
    iCount=cfg.readNumEntry(sEntry, 0);

    // read entries
    for(int i=1; i<=iCount; i++) {
        sEntry.sprintf("%s%d", sKey, i);
        QString sType=cfg.readEntry(sEntry);
        if( sType )
            lst.append(sType);
    }
}


// --- readConfig -------------------------------------------------------------
// Reads the member data from the given config file. It will also set the group
// "Config"
void Cfg::readConfig(Config &config)
{
    // set group
    config.setGroup( "Config" );

    // read scalars
    m_currencySymbol = config.readEntry( "CurrencySymbol", "$" );
    m_useSmallFont = config.readBoolEntry( "UseSmallFont", true );
    m_showLocks = config.readBoolEntry( "ShowLocks", false );
    m_showBalances = config.readBoolEntry( "ShowBalances", false );
    m_openLastBook = config.readBoolEntry( "OpenLastBook", false );
    m_LastBook = config.readEntry("LastBook", "");
    m_showLastTab = config.readBoolEntry( "ShowLastTab", false );
    m_SavePayees = config.readBoolEntry( "SavePayees", false );

    // Account types
    readStringList(config, "AccType", m_AccountTypes);
    if( m_AccountTypes.isEmpty() ) {
        m_AccountTypes+= (const char *)QWidget::tr("Savings");
        m_AccountTypes+= (const char *)QWidget::tr("Checking");
        m_AccountTypes+= (const char *)QWidget::tr("CD");
        m_AccountTypes+= (const char *)QWidget::tr("Money market");
        m_AccountTypes+= (const char *)QWidget::tr("Mutual fund");
        m_AccountTypes+= (const char *)QWidget::tr("Other");
        writeStringList(config, "AccType", m_AccountTypes);
        config.write();
    }

    // Payees
    readStringList(config, "Payee", m_Payees);

    // Read Categories
    QStringList lst;
    readStringList(config, "Category", lst);
    if( lst.isEmpty() ) {
        QString type=QWidget::tr("Expense");
        lst += QWidget::tr( "Automobile" )+";"+type;
        lst += QWidget::tr( "Bills" )+";"+type;
        lst += QWidget::tr( "CDs" )+";"+type;
        lst += QWidget::tr( "Clothing" )+";"+type;
        lst += QWidget::tr( "Computer" )+";"+type;
        lst += QWidget::tr( "DVDs" )+";"+type;
        lst += QWidget::tr( "Electronics" )+";"+type;
        lst += QWidget::tr( "Entertainment" )+";"+type;
        lst += QWidget::tr( "Food" )+";"+type;
        lst += QWidget::tr( "Gasoline" )+";"+type;
        lst += QWidget::tr( "Misc" )+";"+type;
        lst += QWidget::tr( "Movies" )+";"+type;
        lst += QWidget::tr( "Rent" )+";"+type;
        lst += QWidget::tr( "Travel" )+";"+type;

        type=QWidget::tr( "Income" );
        lst += QWidget::tr( "Work" )+";"+type;
        lst += QWidget::tr( "Family Member" )+";"+type;
        lst += QWidget::tr( "Misc. Credit" )+";"+type;

        setCategories(lst);
        writeStringList(config, "Category", lst);
        config.write();
    } else
        setCategories(lst);

    // not dirty
    m_Dirty = false;
}


// --- writeStringList --------------------------------------------------------
// Writes the entries in the control in a configuration file. It is assumed,
// that the group is already set. Key is used to enumerate the entries
void Cfg::writeStringList(Config &cfg, const char *sKey, QStringList &lst)
{
    QString sEntry;
    int iCount=0;
    QStringList::Iterator itr;
    for(itr=lst.begin(); itr!=lst.end(); itr++) {
        sEntry.sprintf("%s%d", sKey, ++iCount);
        cfg.writeEntry(sEntry, *itr );
    }
    sEntry.sprintf("%s_Count", sKey);
    cfg.writeEntry(sEntry, iCount);
}


// --- writeConfig -----------------------------------------------------------
// Writes all config data back to the config file. The group will be set to
// "Config" and the write be commited
void Cfg::writeConfig(Config &config)
{
    // set the group
    config.setGroup( "Config" );

    // write scalars
    config.writeEntry( "CurrencySymbol", m_currencySymbol );
    config.writeEntry( "UseSmallFont", m_useSmallFont );
    config.writeEntry( "ShowLocks", m_showLocks );
    config.writeEntry( "ShowBalances", m_showBalances );
    config.writeEntry( "OpenLastBook", m_openLastBook );
    config.writeEntry( "LastBook", m_LastBook );
    config.writeEntry( "ShowLastTab", m_showLastTab );
    config.writeEntry( "SavePayees", m_SavePayees );

    // write account types
    writeStringList(config, "AccType", m_AccountTypes);

    // write payees
    writeStringList(config, "Payee", m_Payees);

    // write categories
    QStringList lst=getCategories();
    writeStringList(config, "Category", lst );

    // commit write
    config.write();
    m_Dirty=false;
}


// --- getCategories ----------------------------------------------------------
QStringList Cfg::getCategories()
{
    QStringList ret;
    for(Category *itr=m_Categories->first(); itr; itr=m_Categories->next() ) {
        QString Entry = itr->getName() + ";" + (itr->isIncome() ?
						    QWidget::tr( "Income" ) :
						    QWidget::tr( "Expense" ) ) ;
        ret.append( Entry );
    }
    return ret;
}


// --- setCategories ----------------------------------------------------------
void Cfg::setCategories(QStringList &lst)
{
    m_Categories->clear();
    QStringList::Iterator itr;
    for(itr=lst.begin(); itr!=lst.end(); itr++) {
        QStringList split=QStringList::split(";", *itr, true);
        if( split.count()<2 )
            continue;

        bool bIncome = (split[1]==QWidget::tr("Income"));
        m_Categories->append( new Category(split[0], bIncome) );
    }
}


// --- CategoryList ------------------------------------------------------------
CategoryList::CategoryList() : QList<Category>()
{
    setAutoDelete(true);
}
