/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2009 The Opie Team <opie-devel@lists.sourceforge.net>
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

#include <opie2/opimmemo.h>

#include <qpe/categoryselect.h>

#include <qdialog.h>
#include <qlayout.h>
#include <qmultilineedit.h>
#include <qmessagebox.h>

namespace Opie {
namespace Notes {

class editWindowWidget : public QDialog
{
    Q_OBJECT

    public:
        editWindowWidget(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
        ~editWindowWidget();
        void readMemo( const OPimMemo &memo );
        void writeMemo( OPimMemo &memo );

    protected slots:
        void closeEvent( QCloseEvent *e );
        void slotChanged();
        
    private:
        QMultiLineEdit *m_editArea;
        CategorySelect *m_catSelect;
        bool m_changed;

};

}
}
