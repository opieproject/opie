/*
                            This file is part of the OPIE Project

               =.            Copyright (c)  2004 Dan Williams <drw@handhelds.org>
             .=l.
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

#ifndef ENTRYDLG_H
#define ENTRYDLG_H

#include <qdialog.h>

class QLineEdit;

class EntryDlg : public QDialog
{
    Q_OBJECT

public:
    static QString getText( const QString &caption, const QString &label, const QString &text = QString::null,
                            bool *ok = 0, QWidget *parent = 0, const char *name = 0 );

    EntryDlg( const QString &label, QWidget* parent = 0, const char* name = 0, bool modal = true );

    void setText( const QString &text );
    QString getText();

private slots:
    void tryAccept();

private:
    QLineEdit   *m_entry;
};

#endif // EntryDlg_H
