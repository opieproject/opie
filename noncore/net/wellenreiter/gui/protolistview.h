/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
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

#ifndef PROTOLISTVIEW_H
#define PROTOLISTVIEW_H

#include <qscrollview.h>
#include <qdict.h>
class QCheckBox;
class QComboBox;

class QVBox;

class ProtocolListView : public QScrollView
{
  public:
    ProtocolListView( QWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    virtual ~ProtocolListView();

    bool isProtocolChecked( const QString& name );
    QString protocolAction( const QString& name );

  protected:
    virtual void addProtocol( const QString& name );

  private:
    QVBox* vbox;
    bool parse;
};



#endif // PROTOLISTVIEW_H
