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
#ifndef QCOPENVELOPE_H
#define QCOPENVELOPE_H

#ifndef QT_H
#include <qcopchannel_qws.h>
#include <qdatastream.h>
#endif // QT_H

#ifndef QT_NO_COP

#include <qpe/qcopmessage_qws.h>

class QCopEnvelope : public QDataStream {
    QCString ch, msg;
public:
    QCopEnvelope( const QCString& channel, const QCString& message );
    QCopEnvelope(const QCopMessage& message);
    ~QCopEnvelope();
};

/**
 * Wraps this message in a QCopEnvelope for sending.  The original QCopMessage
 * \a message is unchanged.  Once the QCopEnvelope is constructed changes to
 * the original QCopMessage do not affect the message in the envelope.
 */
inline QCopEnvelope::QCopEnvelope(const QCopMessage& message) {
  ch = message.channel();
  msg = message.message();

  QByteArray array(message.data());
  array.detach();
  
  setDevice(new QBuffer(array));
}

#endif

#endif
