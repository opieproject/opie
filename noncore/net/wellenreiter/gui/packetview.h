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

#include <opie2/opcap.h>

class QWidget;
class QLabel;
class QString;
class QSpinBox;
class QTextView;
class QObjectList;
namespace Opie {namespace Net {class OPacket;}}
namespace Opie {namespace Ui {class OListView;}}

class PacketView: public QFrame
{
  Q_OBJECT

  public:
    PacketView( QWidget * parent = 0, const char * name = "PacketView", WFlags f = 0 );

    void add( const Opie::Net::OPacket* p, int size );
    const QString getLog() const;
    void clear();

  public slots:
    void showPacket( int number );
    void activated( QWidget* );

  protected:
    QSpinBox* _number;
    QLabel* _label;
    Opie::Ui::OListView* _list;
    QTextView* _hex;
    QList<const Opie::Net::OPacket> _packets;

  protected:
    void _doSubPackets( QObjectList*, int );
    void _doHexPacket( const Opie::Net::OPacket* );

};

#endif

