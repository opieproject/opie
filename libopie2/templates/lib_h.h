/*
 * this file describes how the usage in namespaces in libopie2 should be done
 * ODP is the namespace for whole opie2 stuff
 * the parts are separated as follow:
 * ONet: opienet2
 * OPim: opiepim2
 * ODb:  opiedb2
 * OCore: opiecore2 
 * OMm:  opiemm2
 * OUi:  opieui2
 * a class MUST start with O - so we make sure that it does not 
 * conflict with other stuff.
 * don't use "using namespace .." in implementations.
 */
/*
                             This file is part of the Opie Project
                             Copyright (C) The Main Author <main-author@whereever.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef THISFILE_H
#define THISFILE_H

/* OPIE */
opie (and for now, qpe) includes here

/* QT */
qte includes here

/* STD */

namespace ODP
{
    namspace ONet {
      classes here
    }
}

#endif

