/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef OPENTEXTDLG_H
#define OPENTEXTDLG_H

#include <qdialog.h>
#include <qlistview.h>

#include <swmgr.h>

class QPixmap;

class OpenTextDlg : public QDialog
{
    Q_OBJECT

public:
    OpenTextDlg( QWidget *parent = 0x0, sword::SWMgr *swordMgr = 0x0, QPixmap *bibleIcon = 0x0,
                 QPixmap *commentaryIcon = 0x0, QPixmap *lexiconIcon = 0x0 );

    QString selectedText() { return m_textList.currentItem()->text( 1 ); };

private:
    QListView m_textList; // List of available texts/modules

    QListViewItem *m_bibles;       // Pointer to bible section header
    QListViewItem *m_lexicons;     // Pointer to lexicon/dictionary section header
    QListViewItem *m_commentaries; // Pointer to commentary section header

private slots:
    void slotItemClicked( QListViewItem *item );
};

#endif
