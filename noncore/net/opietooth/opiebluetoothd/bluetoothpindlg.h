/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2002 The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
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


#include <qdialog.h>
#include "bluetoothpindlgbase.h"

class PinDlg : public PinDlgBase {
        Q_OBJECT
    public:
        PinDlg(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);

        ~PinDlg();
        void setBdAddr( const QString& );
        void setRemoteName( const QString& );
        void setDirection( bool );
        QString pin() const;

    signals:
        void dialogClosed(bool);

    private:
        QString m_bdaddr;
        QString m_remoteName;
        bool m_outgoing;

    protected slots:
        void accept();
        void reject();
        void addnum();
        void delnum();
        void polish();
};
