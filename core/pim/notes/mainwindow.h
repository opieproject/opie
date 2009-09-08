/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
            .=l.             Portions copyright (C) 2002 Henning Holtschneider <hh@holtschneider.com>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <qpe/qpeapplication.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <notesmanager.h>

#include <opie2/opimmainwindow.h>
#include <opie2/owidgetstack.h>
#include <opie2/opimmemo.h>

namespace Opie {
namespace Notes {
    
class MemoListItem : public QListBoxText
{
public:
    MemoListItem ( QListBox *listbox, const QString &text, int uid );
    int uid();
private:
    int m_uid;
};
    
class mainWindowWidget : public Opie::OPimMainWindow
{
    Q_OBJECT

    public:
        QListBox *notesList;

        mainWindowWidget( QWidget *parent=0, const char *name=0, WFlags fl=0 );
        ~mainWindowWidget();
        void refreshList();

    public slots:
        void openFile();
        void deleteFile();

    private:
        int m_selected;
        NotesManager m_manager;
        QString m_curCat;
        
        void populateList( OPimMemoAccess::List &list );
        void editMemo( OPimMemo &memo, bool create );
        void toBeDone(void);
        int currentCatId();
        int currentMemoUid();
        OPimMemo currentMemo();

    private slots:
        void slotItemNew();
        void slotItemEdit();
        void slotItemDuplicate();
        void slotItemDelete();
        void slotItemBeam();
        void slotFind();
        void slotConfigure();
        void slotCategoryChanged( const QString& );

    protected slots:
        void flush();
        void reload();
        int  create();
        bool remove( int uid );
        void beam( int uid);
        void show( int uid );
        void edit( int uid );
        void add( const Opie::OPimRecord& );
};

}
}
