/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
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

#ifndef OTIMEPICKER_H
#define OTIMEPICKER_H

/* OPIE */
#include <opie2/oclickablelabel.h>
#include <opie2/otimepickerbase.h>

/* QT */
#include <qwidget.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qdialog.h>


namespace Opie {
namespace Ui   {

/**
 * A class to pick time. It uses clickable labels
 * internally to allow a quick selection of a time.
 * A time can be selected by two clicks of a user
 *
 * @short A widget to quickly pick a QTime
 * @version 1.0
 * @see QWidget
 * @see QTime
 * @author Hakan Ardo, Stefan Eilers
 */
class OTimePicker : public QWidget
{
    Q_OBJECT

public:
    OTimePicker(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);

public slots:
    void setHour(int h);
    void setMinute(int m);
    void setTime( const QTime& );
    void setTime( int h,  int m );

public:
    QTime time()const;

private:
    QValueList<OClickableLabel *> hourLst;
    QValueList<OClickableLabel *> minuteLst;
    QTime tm;
    struct Private;
    Private *d;

private slots:
    void slotHour(bool b);
    void slotMinute(bool b);

signals:
    /**
     * gets emitted when the time got changed by the user
     */
    void timeChanged(const QTime &);
};

/**
 *
 * @short A small dialog to pick a time
 * @version 1.0
 * @author Stefan Eilers
 *
 **/

class OTimePickerDialog: public OTimePickerDialogBase
{
    Q_OBJECT

public:
    OTimePickerDialog ( QWidget* parent = 0, const char* name = NULL, WFlags fl = 0 );
    ~OTimePickerDialog() { };

    QTime time()const;

public slots:
    void setTime( const QTime& time );
    void setHour( const QString& hour );
    void setMinute( const QString& minute );

private:
    OTimePicker *m_timePicker;
    QTime m_time;
    class Private;
    Private* d;
};

}
}

/* for Qt2  */
#if ( QT_VERSION-0 >= 0x030000 )
#error "Fix the UI File to use namespaces"
#endif
#endif
