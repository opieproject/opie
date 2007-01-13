/*
                             This file is part of the Opie Project
                             Copyright (C) 2004 Holger Freyther <freyther@handhelds.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "opimcontactsortvector.h"
#include <opie2/ocontactaccess.h>

namespace Opie {
namespace Internal {
OPimContactSortVector::OPimContactSortVector( uint size, bool asc, int sort )
    : OPimSortVector<OPimContact>( size, asc, sort ) {}

int OPimContactSortVector::compareItems( const OPimContact& left,
                                         const OPimContact& right ) {
    if ( left.uid() == right.uid() )
        return 0;

    bool soTitle, soSummary, soFirstName, soMiddleName, soSuffix, soEmail,
        soNick, soFileAs, soAnni, soBirth, soGender;
    soTitle = soSummary = soFirstName = soMiddleName = soSuffix = soEmail =
     soNick = soFileAs = soAnni = soBirth = soGender = false;
    int ret = 0;
    bool asc = sortAscending();

    switch( sortOrder() ) {
    case OPimContactAccess::SortSummary:
        ret = testString( left.fileAs(), right.fileAs() );
        soSummary = true;
        break;
    case OPimContactAccess::SortTitle:
        ret = testString( left.title(), right.title() );
        soTitle = true;
        break;
    case OPimContactAccess::SortFirstName:
        ret = testString( left.firstName(), right.firstName() );
        soFirstName = true;
        break;
    case OPimContactAccess::SortMiddleName:
        ret = testString( left.middleName(), right.middleName() );
        soMiddleName = true;
        break;
    case OPimContactAccess::SortLastName:
        ret = testString( left.lastName(), right.lastName() );
        break;
    case OPimContactAccess::SortSuffix:
        ret = testString( left.suffix(), right.suffix() );
        soSuffix = true;
        break;
    case OPimContactAccess::SortEmail:
        ret = testString( left.defaultEmail(), right.defaultEmail() );
        soEmail = true;
        break;
    case OPimContactAccess::SortNickname:
        ret = testString( left.nickname(), right.nickname() );
        soNick = true;
        break;
    case OPimContactAccess::SortFileAsName:
        ret = testString( left.fileAs(), right.fileAs() );
        soFileAs = true;
        break;
    case OPimContactAccess::SortAnniversary:
        ret = testDate( left.anniversary(), right.anniversary() );
        soAnni = true;
        break;
    case OPimContactAccess::SortByDate:
    case OPimContactAccess::SortBirthday:
        ret = testDate( left.birthday(), right.birthday() );
        soBirth = true;
        break;
    case OPimContactAccess::SortGender:
        ret = testString( left.gender(), right.gender() );
        soGender = true;
        break;
    case OPimContactAccess::SortBirthdayWithoutYear:
        // This doesn't actually just sort by the date without year,
        // it actually works out the days until the next occurrence,
        // which is more useful since it will work correctly when
        // crossing year boundaries. - Paul Eggleton Dec 2006
        ret = testDaysUntilNextDate( left.birthday(), right.birthday() );
        break;
    case OPimContactAccess::SortAnniversaryWithoutYear:
        // (as above)
        ret = testDaysUntilNextDate( left.anniversary(), right.anniversary() );
        break;
    }

    /* twist to honor ascending/descending setting as QVector only sorts ascending*/
    if ( !asc )
        ret *= -1;

    //  Maybe differentiate as in OPimTodoSortVector ### FIXME
    //  if( ret )
    return ret;
}

}
}
