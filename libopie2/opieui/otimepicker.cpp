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

/* QT */
#include <qlayout.h>
#include <qlineedit.h>

/* OPIE */
#include <opie2/otimepicker.h>

using namespace Opie;

/**
 * Constructs the widget
 * @param parent The parent of the OTimePicker
 * @param name The name of the object
 * @param fl Window Flags
 */
OTimePicker::OTimePicker(QWidget* parent, const char* name, Qt::WFlags fl)
            :QWidget(parent,name,fl)
{
    QVBoxLayout *vbox=new QVBoxLayout(this);

    OClickableLabel *r;
    QString s;

    // Hour Row
    QWidget *row=new QWidget(this);
    QHBoxLayout *l=new QHBoxLayout(row);
    vbox->addWidget(row);

    for (int i=0; i<24; i++)
    {
        r=new OClickableLabel(row);
        hourLst.append(r);
        s.sprintf("%.2d",i);
        r->setText(s);
        r->setToggleButton(true);
        r->setAlignment(AlignHCenter | AlignVCenter);
        l->addWidget(r);
        connect(r, SIGNAL(toggled(bool)),
                this, SLOT(slotHour(bool)));

        if (i==11)
        { // Second row
            row=new QWidget(this);
            l=new QHBoxLayout(row);
            vbox->addWidget(row);
        }
    }

    // Minute Row
    row=new QWidget(this);
    l=new QHBoxLayout(row);
    vbox->addWidget(row);

    for (int i=0; i<60; i+=5)
    {
        r=new OClickableLabel(row);
        minuteLst.append(r);
        s.sprintf("%.2d",i);
        r->setText(s);
        r->setToggleButton(true);
        r->setAlignment(AlignHCenter | AlignVCenter);
        l->addWidget(r);
        connect(r, SIGNAL(toggled(bool)),
                this, SLOT(slotMinute(bool)));
    }
}

/**
 * This method return the current time
 * @return the time
 */
QTime OTimePicker::time()const
{
    return tm;
}

void OTimePicker::slotHour(bool b)
{

    OClickableLabel *r = (OClickableLabel *) sender();

    if (b)
    {
        QValueListIterator<OClickableLabel *> it;
        for (it=hourLst.begin(); it!=hourLst.end(); it++)
        {
            if (*it != r) (*it)->setOn(false);
            else tm.setHMS((*it)->text().toInt(), tm.minute(), 0);
        }
        emit timeChanged(tm);
    }
    else
    {
        r->setOn(true);
    }

}

void OTimePicker::slotMinute(bool b)
{

    OClickableLabel *r = (OClickableLabel *) sender();

    if (b)
    {
        QValueListIterator<OClickableLabel *> it;
        for (it=minuteLst.begin(); it!=minuteLst.end(); it++)
        {
            if (*it != r) (*it)->setOn(false);
            else tm.setHMS(tm.hour(),(*it)->text().toInt(), 0);
        }
        emit timeChanged(tm);
    }
    else
    {
        r->setOn(true);
    }

}

/**
 * Method to set the time. No signal gets emitted during this method call
 * Minutes must be within 5 minutes step starting at 0 ( 0,5,10,15,20... )
 * @param t The time to be set
 */
void OTimePicker::setTime( const QTime& t)
{
    setTime( t.hour(), t.minute() );
}

/**
 * Method to set the time. No signal gets emitted during this method call
 * @param h The hour
 * @param m The minute. Minutes need to set by 5 minute steps
 */
void OTimePicker::setTime( int h,  int m )
{
    setHour(h);
    setMinute(m);
}

/*
 * FIXME round minutes to the 5 minute arrangement -zecke
 */
/**
 * Method to set the minutes
 * @param m minutes
 */
void OTimePicker::setMinute(int m)
{

    QString minute;
    minute.sprintf("%.2d",m);

    QValueListIterator<OClickableLabel *> it;
    for (it=minuteLst.begin(); it!=minuteLst.end(); it++)
    {
        if ((*it)->text() == minute) (*it)->setOn(true);
        else (*it)->setOn(false);
    }

    tm.setHMS(tm.hour(),m,0);
}

/**
 * Method to set the hour
 */
void OTimePicker::setHour(int h)
{

    QString hour;
    hour.sprintf("%.2d",h);

    QValueListIterator<OClickableLabel *> it;
    for (it=hourLst.begin(); it!=hourLst.end(); it++)
    {
        if ((*it)->text() == hour) (*it)->setOn(true);
        else (*it)->setOn(false);
    }
    tm.setHMS(h,tm.minute(),0);
}


/**
 * This is a modal Dialog.
 *
 * @param parent The parent widget
 * @param name The name of the object
 * @param fl Possible window flags
 */
OTimePickerDialog::OTimePickerDialog ( QWidget* parent, const char* name, WFlags fl )
        : OTimePickerDialogBase (parent , name, true , fl)
{

    connect ( m_timePicker, SIGNAL( timeChanged(const QTime&) ),
              this, SLOT( setTime(const QTime&) ) );
    connect ( minuteField, SIGNAL( textChanged(const QString&) ),
              this, SLOT ( setMinute(const QString&) ) );
    connect ( hourField, SIGNAL( textChanged(const QString&) ),
              this, SLOT ( setHour(const QString&) ) );

}

/**
 * @return the time
 */
QTime OTimePickerDialog::time()const
{
    return m_time;
}

/**
 * Set the time to time
 * @param time The time to be set
 */
void OTimePickerDialog::setTime( const QTime& time )
{
    m_time = time;

    m_timePicker->setHour ( time.hour() );
    m_timePicker->setMinute( time.minute() );

    // Set Textfields
    if ( time.hour() < 10 )
        hourField->setText( "0" + QString::number( time.hour() ) );
    else
        hourField->setText( QString::number( time.hour() ) );

    if ( time.minute() < 10 )
        minuteField->setText( "0" + QString::number( time.minute() ) );
    else
        minuteField->setText( QString::number( time.minute() ) );

}

/**
 * This method takes the current minute and tries to set hour
 * to hour. This succeeds if the resulting date is valid
 * @param hour The hour as a string
 */
void OTimePickerDialog::setHour ( const QString& hour )
{
    if ( QTime::isValid ( hour.toInt(),  m_time.minute() , 00 ) )
    {
        m_time.setHMS ( hour.toInt(),  m_time.minute() , 00 );
        setTime ( m_time );
    }

}

/**
 * Method to set a new minute. It tries to convert the string to int and
 * if the resulting date is valid a new date is set.
 * @see setHour
 */
void OTimePickerDialog::setMinute ( const QString& minute )
{
    if ( QTime::isValid ( m_time.hour(), minute.toInt(), 00 ) )
    {
        m_time.setHMS ( m_time.hour(), minute.toInt(), 00 );
        setTime ( m_time );
    }
}
