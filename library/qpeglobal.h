/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002,2003,2004 Holger Hans Peter Freyther <freyther@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can 
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#ifndef QPE_GLOBAL_H
#define QPE_GLOBAL_H

/**
 * Defines for used compiler attributes
 *
 */
#if defined(Q_OS_MACX)
#define QPE_WEAK_SYMBOL __attribute__((weak_import))
#define QPE_SYMBOL_USED
#define QPE_SYMBOL_UNUSED

#elif defined(_OS_UNIX_)
#define QPE_WEAK_SYMBOL  __attribute__((weak))
#define QPE_SYMBOL_USED __attribute__((used))
#define QPE_SYMBOL_UNUSED __attribute__((unused))


#else // defined(Q_OS_WIN32)
#define QPE_WEAK_SYMBOL
#define QPE_SYMBOL_USED
#define QPE_SYMBOL_UNUSED
#endif



#endif
