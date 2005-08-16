/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002-2005  Kurt Korbatits <support@midget.net.au>
           .>+-=             Copyright (c)  2005  L. Potter <lpotter@trolltech.com>
 _;:,     .>    :=|.         This program is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.        
    .i_,=:_.      -<s.       This program is distributed in the hope that  
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-        
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.BIN. 
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef QSMB_H
#define QSMB_H

#include "qsmbbase.h"

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>

#include <qlistview.h>
#include <pthread.h>

#include <qlistview.h>

class Qsmb : public FormQPESMBBase
{ 
   Q_OBJECT

public:
   static QString appName() { return QString::fromLatin1("opie-smb"); }
   Qsmb( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
   ~Qsmb();
   void scan();
   void DoIt();

private:
   QListViewItem *top_element;
   QComboBox *hosts;
   pthread_t tpid;
   bool scanning;
   bool isMounted(const QString &);
   QString getMount(const QString &);

public slots:
   void clear();
	 void scanClicked();
   void hostSelected(int);
   void DoItClicked();
   void umountIt();
   QString out;
   bool runCommand(const QStringList &);

private slots:
   void TextViewClicked(QListViewItem*);
};
void* runit(void *arg);
void* runitm(void *arg);

#endif // QSMB_H
