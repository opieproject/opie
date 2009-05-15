/*
                             This file is part of the Opie Project
                             Copyright (C) 2009 The Opie Team <opie-devel@handhelds.org>
              =.             
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


#include <opie2/odatebookaccess.h>

#include "bookmanager.h"
#include "mainwindow.h"
#include "view.h"

using namespace Opie;
using namespace Opie::Datebook;

View::View( MainWindow* window, QWidget* )    {
    m_win = window;
}

View::~View() {
}

QDate View::dateFromWeek( int week, int year, bool startOnMon ) {

}

// For Weeks that start on Monday... (EASY!)
// At the moment we will use the ISO 8601 method for computing the week.
// Granted, other countries use other methods, but we aren't doing any
// Locale stuff at the moment. Without locale info it is too hard to
// guess what method should be used. So, this should pass. This
// algorithim is public domain and available at:
// http://personal.ecu.edu/mccartyr/ISOwdALG.txt
// the week number is returned, and the year number is returned in year
// for Instance 2001/12/31 is actually the first week in 2002.
// There is a more mathematical definition, it should be implemented.

// For Weeks that start on Sunday... (ahh... home rolled)
// okay, if Jan 1 is on Friday or Saturday, it will go to the previous
// week.

/*!
 * The week number and year number of a date may actually be different
 * then the date itself as specified by the ISO 8601 standard. For
 * example if Jan 1 year falls on a Friday, the week number will
 * be the last week number for the previous year (52 of year - 1). Another
 * example is that the date 2001-12-31 falls on a Monday, its week number
 * is 1 and its year is 2002. This function provides stripped down
 * version of the alogrithm described in
 * http://personal.ecu.edu/mccartyr/ISOwdALG.txt to provide the correct
 * week number and year number for a given date.
 *
 * \param d The date that you want to know the week number for.
 * \param week A reference to the variable you want to store the week
 *        number in.
 * \param year A reference to the variable you want to store the year
 *        number in.
 * \param startOnMonday Inform the function whether weeks start on
 *        Sunday or Monday. Set to true if your weeks start on Monday.
 * \return false if the supplied date is invalid, true otherwise.
 */
bool View::calcWeek( const QDate &d, int &week, int &year,
           bool startOnMonday )
{
    int weekNumber = -1;
    int yearNumber = -1;

    int jan1WeekDay = QDate(d.year(), 1, 1).dayOfWeek();
    int dayOfWeek = d.dayOfWeek();

    if ( !d.isValid() )
        return false;

    if ( startOnMonday ) {
        // find the Jan1Weekday;
        if ( d.dayOfYear() <= ( 8 - jan1WeekDay) && jan1WeekDay > 4 ) {
            yearNumber = d.year() - 1;
            if ( jan1WeekDay == 5
                 || ( jan1WeekDay == 6 && QDate::leapYear(yearNumber) ) )
                weekNumber = 53;
            else
                weekNumber = 52;
        } else
            yearNumber = d.year();

        if ( yearNumber == d.year() ) {
            int totalDays = 365;
            if ( QDate::leapYear(yearNumber) )
                totalDays++;
            if ( ((totalDays - d.dayOfYear()) < (4 - dayOfWeek) )
                 || (jan1WeekDay == 7) && (totalDays - d.dayOfYear()) < 3)
            {
                yearNumber++;
                weekNumber = 1;
            }
        }
        if ( yearNumber == d.year() ) {
            int j = d.dayOfYear() + (7 - dayOfWeek) + ( jan1WeekDay - 1 );
            weekNumber = j / 7;
            if ( jan1WeekDay > 4 )
                weekNumber--;
        }
    } else {
        // it's better to keep these cases separate...
        if ( d.dayOfYear() <= (7 - jan1WeekDay) && jan1WeekDay > 4
             && jan1WeekDay != 7 )
        {
            yearNumber = d.year() - 1;
            if ( jan1WeekDay == 6
                 || (jan1WeekDay == 7 && QDate::leapYear(yearNumber) ) )
            {
                weekNumber = 53;
            } else
                weekNumber = 52;
        } else
            yearNumber = d.year();

        if ( yearNumber == d.year() ) {
            int totalDays = 365;
            if ( QDate::leapYear( yearNumber ) )
                totalDays++;
            if ( ((totalDays - d.dayOfYear()) < (4 - dayOfWeek % 7)) ) {
                yearNumber++;
                weekNumber = 1;
            }
        }
        if ( yearNumber == d.year() ) {
            int j = d.dayOfYear() + (7 - dayOfWeek % 7) + ( jan1WeekDay - 1 );
            weekNumber = j / 7;

            if ( jan1WeekDay > 4 )
                weekNumber--;
        }
    }
    year = yearNumber;
    week = weekNumber;
    return true;
}

QString View::occurrenceDesc( const OPimOccurrence &occ ) {
    QString summary = occ.summary();
    if( summary.isNull() )
        return occ.toEvent().description();
    else
        return summary;
}

void View::loadConfig( Config* conf ) {
    doLoadConfig( conf );
}

void View::saveConfig( Config* conf ) {
    doSaveConfig( conf );
}

void View::popup( const OPimOccurrence &ev, const QPoint &pt ) {
    m_win->viewPopup( ev, pt );
}

void View::add( const QDateTime& start, const QDateTime& end, const QString &desc ) {
    m_win->viewAdd( start, end, desc );
}

void View::add( const OPimEvent &ev ) {
    m_win->viewAdd( ev );
}

void View::edit( int uid ) {
    m_win->edit( uid );
}

void View::remove( int uid ) {
    m_win->remove( uid );
}

void View::showDayView() {
    m_win->showDayView();
}

ODateBookAccess::List View::allEvents() const{
    return m_win->manager()->allRecords();
}

OPimOccurrence::List View::events( const QDate& from, const QDate& to ) {
    return m_win->manager()->list( from, to );
}

OPimEvent View::event( int uid ) const{
    return m_win->manager()->event( uid );
}

bool View::isAP()const {
    return m_win->viewAP();
}

bool View::weekStartOnMonday()const {
    return m_win->viewStartMonday();
}

QList<OPimRecord> View::records( const QDate& on ) {
    return m_win->manager()->records( on, on );
}

QList<OPimRecord> View::records( const QDate& on, const QDate& to ) {
    return m_win->manager()->records( on, to );
}
