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

#ifndef QCOP_MESSAGE_H
#define QCOP_MESSAGE_H

#include <qdatastream.h>
#include <qbuffer.h>

class QCopMessage : public QDataStream {
 public:
  QCopMessage();
  QCopMessage(const QCString& channel, const QCString& message);
  QCopMessage(const QCopMessage& orig);
  ~QCopMessage();

  void setChannel(QCString& channel) { m_Channel = channel; }
  QCString channel() const { return m_Channel; }
  void setMessage(QCString& message) { m_Message = message; }
  QCString message() const { return m_Message; }
  const QByteArray data() const;

  QCopMessage& operator=(const QCopMessage& orig);

 private:
  QCString m_Channel;
  QCString m_Message;
};

// ### No need to inline, just maintaining binary compatability
inline QCopMessage::QCopMessage() : QDataStream(new QBuffer()) {
    device()->open(IO_WriteOnly);
}

inline QCopMessage::QCopMessage(const QCString& channel, const QCString& message) 
  : QDataStream(new QBuffer()), m_Channel(channel), m_Message(message) {
  device()->open(IO_WriteOnly);
}

inline QCopMessage::QCopMessage(const QCopMessage& orig) : QDataStream() {
  // The QBuffer is going to share the byte array, so it will keep the
  // data pointer even when this one goes out of scope.
  QByteArray array(((QBuffer*)orig.device())->buffer());
  array.detach();
  setDevice(new QBuffer(array));
  device()->open(IO_Append);

  m_Channel = orig.channel();
  m_Message = orig.message();
}

inline QCopMessage& QCopMessage::operator=(const QCopMessage& orig) {
  if (device()) {
    delete device();
    unsetDevice();
  }

  // The QBuffer is going to share the byte array, so it will keep the
  // data pointer even when this one goes out of scope.
  QByteArray array(((QBuffer*)orig.device())->buffer());
  array.detach();
  setDevice(new QBuffer(array));
  device()->open(IO_Append);

  m_Channel = orig.channel();
  m_Message = orig.message();  

  return *this;
}

inline const QByteArray QCopMessage::data() const {
  return ((QBuffer*)device())->buffer();
}

inline QCopMessage::~QCopMessage() {
  // If we still have our QBuffer, clean it up...
  if (device())
    delete device();
  unsetDevice();
}

#endif
