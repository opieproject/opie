/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
#ifndef QRR_H
#define QRR_H

#include <qobject.h>
#include <qlist.h>
#include <qcstring.h>

class QFile;
class QDialog;
class QLabel;
class QProgressBar;
class QWidget;

class QueuedRequestRunner : public QObject
{
    Q_OBJECT
public:
    QueuedRequestRunner( QFile *f, QWidget *parent );
    virtual ~QueuedRequestRunner();

    void desktopMessage( const QCString &message, const QByteArray &data );

    bool readyToDelete;
    bool waitingForMessages;

signals:
    void finished();

public slots:
    void process();

private:
    int countSteps();
    bool process( bool counting );

    QFile *file;
    QList<QCString> waitMsgs;
    QDialog      *progressDialog;
    QLabel       *progressLabel;
    QProgressBar *progressBar;
    int steps;
    QCString action;
};


#endif // QRR_H

