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

#ifndef CFG_H
#define CFG_H

#include <qstring.h>
#include <qlist.h>
#include <qstringlist.h>
class Config;

// --- Category ---------------------------------------------------------------
class Category
{
    public:
        // --- Constructor:
        Category(QString &sName, bool bIncome=false) { _sName=sName; _bIncome=bIncome; }

        // members
        QString &getName() { return(_sName); }
        bool isIncome() { return(_bIncome); }
        void setName(QString &sName) { _sName=sName; }
        void setIncome(bool bIncome) { _bIncome=bIncome; }

    private:
        QString _sName;
        bool _bIncome;
};

class CategoryList : public QList<Category>
{
    public:
        // --- Constructor
        CategoryList();
};


// --- Cfg --------------------------------------------------------------------
class Cfg
{
    public:
        // --- Constructor
        Cfg();

        // --- members
        bool getShowLocks() { return(_showLocks); }
        void setShowLocks(bool n) { _showLocks=n; }
        bool getShowBalances() { return(_showBalances); }
        void setShowBalances(bool n) { _showBalances=n; }
        QString &getCurrencySymbol() { return(_currencySymbol); }
        void setCurrencySymbol(QString n) {_currencySymbol= n; }
        void setCurrencySymbol(const char *n) { _currencySymbol=n; }
        QStringList &getAccountTypes() { return(_AccountTypes); }

        // --- Categories
        QStringList getCategories();
        void setCategories(QStringList &lst);
        CategoryList *getCategoryList() { return(_pCategories); }

        // --- last book
        void setOpenLastBook(bool openLastBook) { _openLastBook=openLastBook; }
        bool isOpenLastBook() { return(_openLastBook); }
        void setLastBook(const QString &lastBook) { _sLastBook=lastBook; }
        QString &getLastBook() { return(_sLastBook); }

        // --- last tab
        void setShowLastTab(bool showLastTab) { _showLastTab=showLastTab; }
        bool isShowLastTab() { return(_showLastTab); }

        // --- reads data from config file
        void readConfig(Config &cfg);

        // --- writes data to config file
        void writeConfig(Config &cfg);

        // --- reads list from config file
        static void readStringList(Config &cfg, const char *sKey, QStringList &lst);

        // --- writes list in configuration file
        static void writeStringList(Config &cfg, const char *sKey, QStringList &lst);



    private:
        QString  _currencySymbol;
		bool _showLocks;
		bool _showBalances;
        bool _openLastBook;
        bool _showLastTab;
        QString _sLastBook;
        QStringList _AccountTypes;
        CategoryList *_pCategories;
};

#endif
