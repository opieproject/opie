/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef __TASK_H__
#define __TASK_H__

#include <qpe/palmtoprecord.h>
#include <qpe/stringutil.h>

#include <qvaluelist.h>
#include <qdatetime.h>

class TaskPrivate;
class QPC_EXPORT Task : public Qtopia::Record
{
public:
    Task();
    Task( const QMap<int, QString> &fromMap );
    ~Task();

    QMap<int, QString> toMap() const;

    static void writeVCalendar( const QString &filename, const QValueList<Task> &tasks);
    static void writeVCalendar( const QString &filename, const Task &task);
    static QValueList<Task> readVCalendar( const QString &filename );

    void setPriority( int priority ) { mPriority = priority; }
    int priority() const { return mPriority; }

//     void setCategory( const QString& category )
//     { mCategory = category.stripWhiteSpace(); }
//     const QString &category() const { return mCategory; }

    void setDescription( const QString& description )
    { mDesc = Qtopia::simplifyMultiLineSpace(description); }
    const QString &description() const { return mDesc; }

    void setDueDate( const QDate& date, bool hasDue ) { mDueDate = date; mDue = hasDue; }
    const QDate &dueDate() const { return mDueDate; }
    bool hasDueDate() const { return mDue; }
    void setHasDueDate( bool b ) { mDue = b; }
    
    void setCompleted( bool b ) { mCompleted = b; }
    bool isCompleted() const { return mCompleted; }

    void save( QString& buf ) const;
    bool match( const QRegExp &r ) const;

private:
    Qtopia::UidGen &uidGen() { return sUidGen; }
    static Qtopia::UidGen sUidGen;

    bool mDue;
    QDate mDueDate;
    bool mCompleted;
    int mPriority;
    QString mDesc;
    TaskPrivate *d;
};

#endif
