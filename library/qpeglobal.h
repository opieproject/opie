/*
                             This file is part of the OPIE Project
			     Copyright (c)  2002,2003,2004 Holger Hans Peter Freyther <freyther@handhelds.org>
			     Copyright (c)  2002,2003,2004 Stefan Eilers <eilers@handhelds.org>

               =.
             .=l.
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

#ifndef QPE_GLOBAL_DEFINES_H
#define QPE_GLOBAL_DEFINES_H

/**
 * Defines for used compiler attributes
 *
 */

/*
 * commons
 */
#define QPE_DEPRECATED


#if defined(Q_OS_MACX)
#define QPE_WEAK_SYMBOL __attribute__((weak_import))
#define QPE_SYMBOL_USED
#define QPE_SYMBOL_UNUSED
#define QPE_EXPORT_SYMBOL

#elif defined(_OS_UNIX_)
#define QPE_WEAK_SYMBOL  __attribute__((weak))
#define QPE_SYMBOL_USED __attribute__((used))
#define QPE_SYMBOL_UNUSED __attribute__((unused))
#define QPE_EXPORT_SYMBOL


/*
 * mark method as deprecated
 */
#if __GNUC__ - 0 > 3 || (__GNUC__ - 0 == 3 && __GNUC_MINOR__ - 0 >= 2)
  /* gcc >= 3.2 */
#undef  QPE_DEPRECATED
#define QPE_DEPRECATED __attribute__((deprecated))
#endif

/*
 * Defined if Compiler supports attributes
 */
#ifdef GCC_SUPPORTS_VISIBILITY
#undef QPE_EXPORT_SYMBOL
#define QPE_EXPORT_SYMBOL __attribute__((visibility("default")))
#endif



#else // defined(Q_OS_WIN32)
#define QPE_WEAK_SYMBOL
#define QPE_SYMBOL_USED
#define QPE_SYMBOL_UNUSED
#define QPE_EXPORT_SYMBOL
#endif


#define QTOPIA_MERGED_METHOD(method, version) method QPE_WEAK_SYMBOL;
#define NOT_IN_SHARP(method) method QPE_WEAK_SYMBOL;
#define NOT_IN_QPE(method) method QPE_WEAK_SYMBOL;

#endif
