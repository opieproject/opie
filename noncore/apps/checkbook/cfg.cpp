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

#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <qpe/resource.h>
#include <qpe/config.h>

#include "cfg.h"

// --- Cfg --------------------------------------------------------------------
Cfg::Cfg()
{
    _currencySymbol="$";
    _showLocks=FALSE;
    _showBalances=FALSE;
    _pCategories=new CategoryList();
}

// --- readStringList ---------------------------------------------------------
// Reads the entries for the control from a configuration file and returns
// them in a StringList. Later this list can be used to create the control. It
// is assumed, that the group is already set. Key is used to enumerate the
// entries.
void Cfg::readStringList(Config &cfg, const char *sKey, QStringList &lst)
{
qDebug( "%s", sKey );

    QString sEntry;
    int iCount;

    // read count of elements
    sEntry.sprintf("%s_Count", sKey);
    iCount=cfg.readNumEntry(sEntry, 0);

    // read entries
    for(int i=1; i<=iCount; i++) {
        sEntry.sprintf("%s%d", sKey, i);
        QString sType=cfg.readEntry(sEntry);
        if( sType!=NULL )
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
	_currencySymbol = config.readEntry( "CurrencySymbol", "$" );
	_showLocks = config.readBoolEntry( "ShowLocks", FALSE );
	_showBalances = config.readBoolEntry( "ShowBalances", FALSE );
    _openLastBook = config.readBoolEntry( "OpenLastBook", FALSE );
    _sLastBook = config.readEntry("LastBook", "");
    _showLastTab = config.readBoolEntry( "ShowLastTab", FALSE );

    // Account types
    readStringList(config, "AccType", _AccountTypes);
    if( _AccountTypes.isEmpty() ) {
        _AccountTypes+= (const char *)QWidget::tr("Savings");
        _AccountTypes+= (const char *)QWidget::tr("Checking");
	    _AccountTypes+= (const char *)QWidget::tr("CD");
	    _AccountTypes+= (const char *)QWidget::tr("Money market");
	    _AccountTypes+= (const char *)QWidget::tr("Mutual fund");
	    _AccountTypes+= (const char *)QWidget::tr("Other");
        writeStringList(config, "AccType", _AccountTypes);
        config.write();
    }

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
    } else {
        setCategories(lst);
    }
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
    config.writeEntry( "CurrencySymbol", _currencySymbol );
    config.writeEntry( "ShowLocks", _showLocks );
    config.writeEntry( "ShowBalances", _showBalances );
    config.writeEntry( "OpenLastBook", _openLastBook );
    config.writeEntry( "LastBook", _sLastBook );
    config.writeEntry( "ShowLastTab", _showLastTab );

    // write account types
    writeStringList(config, "AccType", _AccountTypes);

    // write categories
    QStringList lst=getCategories();
    writeStringList(config, "Category", lst );

    // commit write
    config.write();
}


// --- getCategories ----------------------------------------------------------
QStringList Cfg::getCategories()
{
    QStringList ret;
    for(Category *itr=_pCategories->first(); itr; itr=_pCategories->next() ) {
        QString sEntry;
        sEntry.sprintf("%s;%s", (const char *)itr->getName(), (const char *)(itr->isIncome() ? QWidget::tr("Income") : QWidget::tr("Expense")) );
        ret.append(sEntry);
    }
    return(ret);
}


// --- setCategories ----------------------------------------------------------
void Cfg::setCategories(QStringList &lst)
{
    _pCategories->clear();
    QStringList::Iterator itr;
    for(itr=lst.begin(); itr!=lst.end(); itr++) {
        QStringList split=QStringList::split(";", *itr, true);
        if( split.count()<2 ) continue;
        bool bIncome= (split[1]==QWidget::tr("Income"));
        _pCategories->append( new Category(split[0], bIncome) );
    }
}


// --- CategoryList ------------------------------------------------------------
CategoryList::CategoryList() : QList<Category>()
{
    setAutoDelete(true);
}
