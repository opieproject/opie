/**********************************************************************
** Copyright (C) 2002-2004 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Wellenreiter II.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef PACKETVIEW_H
#define PACKETVIEW_H

#include <qlist.h>
#include <qframe.h>

class QString;
class QSpinBox;
class QLabel;
class QTextView;
namespace Opie {namespace Net {class OPacket;}}

class PacketView: public QFrame
{

  public:
    PacketView( QWidget * parent = 0, const char * name = "PacketView", WFlags f = 0 );

    void add( Opie::Net::OPacket* p );
    const QString getLog() const;
    void clear();

  protected:

    QSpinBox* _number;
    QLabel* _label;
    QLabel* _list;
    QTextView* _hex;
    QList<Opie::Net::OPacket> _packets;

};

#endif

