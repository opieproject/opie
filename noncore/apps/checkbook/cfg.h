/*
                     This file is part of the OPIE Project
               =.
      .=l.            Copyright (c)  2002 Dan Williams <drw@handhelds.org>
     .>+-=
_;:,   .>  :=|.         This file is free software; you can
.> <`_,  > .  <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This file is distributed in the hope that
  + . -:.    =       it will be useful, but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=    =    ;      Public License for more details.
++=  -.   .`   .:
:   = ...= . :.=-        You should have received a copy of the GNU
-.  .:....=;==+<;          General Public License along with this file;
 -_. . .  )=. =           see the file COPYING. If not, write to the
  --    :-=`           Free Software Foundation, Inc.,
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
        Category(QString &sName, bool bIncome=false) { m_Name=sName; m_Income=bIncome; }

        // members
        QString &getName() { return m_Name; }
        bool isIncome() { return m_Income; }
        void setName(QString &sName) { m_Name=sName; }
        void setIncome(bool bIncome) { m_Income=bIncome; }

    private:
        QString m_Name;
        bool m_Income;
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
        Cfg();
        ~Cfg();

        bool getUseSmallFont() { return(m_useSmallFont); }
        void setUseSmallFont(bool n) { m_useSmallFont=n; }
        bool getShowLocks() { return(m_showLocks); }
        void setShowLocks(bool n) { m_showLocks=n; }
        bool getShowBalances() { return(m_showBalances); }
        void setShowBalances(bool n) { m_showBalances=n; }
        QString &getCurrencySymbol() { return(m_currencySymbol); }
        void setCurrencySymbol(QString n) {m_currencySymbol= n; }
        void setCurrencySymbol(const char *n) { m_currencySymbol=n; }
        QStringList &getAccountTypes() { return(m_AccountTypes); }

        QStringList &getPayees() { return(m_Payees); }
        bool getSavePayees() { return(m_SavePayees); }
        void setSavePayees(bool bSave) { m_SavePayees=bSave; }

        QStringList getCategories();
        void setCategories(QStringList &lst);
        CategoryList *getCategoryList() { return(m_Categories); }

        void setOpenLastBook(bool openLastBook) { m_openLastBook=openLastBook; }
        bool isOpenLastBook() { return(m_openLastBook); }
        void setLastBook(const QString &lastBook) { m_LastBook=lastBook; }
        QString &getLastBook() { return(m_LastBook); }

        void setShowLastTab(bool showLastTab) { m_showLastTab=showLastTab; }
        bool isShowLastTab() { return(m_showLastTab); }

        void readConfig(Config &cfg);

        void writeConfig(Config &cfg);

        bool isDirty() { return(m_Dirty); }
        void setDirty(bool bDirty) { m_Dirty=bDirty; }

    protected:
        static void readStringList(Config &cfg, const char *sKey, QStringList &lst);
        static void writeStringList(Config &cfg, const char *sKey, QStringList &lst);

    private:
        QString m_currencySymbol;
        bool m_useSmallFont;
        bool m_showLocks;
        bool m_showBalances;
        bool m_openLastBook;
        bool m_showLastTab;
        bool m_Dirty;
        bool m_SavePayees;
        QString m_LastBook;
        QStringList m_AccountTypes;
        CategoryList *m_Categories;
        QStringList m_Payees;
};
#endif
