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

#ifndef SCANLIST_H
#define SCANLIST_H

#include "gps.h"

/* OPIE */
#include <opie2/olistview.h>
#include <opie2/onetutils.h>

/* QT */
#include <qtextstream.h>

class QString;
class MScanListItem;

class MScanListView: public OListView
{
  Q_OBJECT

  public:
    MScanListView( QWidget* parent = 0, const char* name = 0 );
    virtual ~MScanListView();

    virtual OListViewItem* childFactory();
    virtual void serializeTo( QDataStream& s ) const;
    virtual void serializeFrom( QDataStream& s );

  public slots:
    void addNewItem( const QString& type, const QString& essid, const OMacAddress& macaddr, bool wep, int channel, int signal, const GpsLocation& location );
    void addService( const QString& name, const OMacAddress& macaddr, const QString& ip );

    void fromDStraffic( const OMacAddress& from, const OMacAddress& to, const OMacAddress& via );
    void toDStraffic( const OMacAddress& from, const OMacAddress& to, const OMacAddress& via );
    void WDStraffic( const OMacAddress& from, const OMacAddress& to, const OMacAddress& viaFrom, const OMacAddress& viaTo );
    void IBSStraffic( const OMacAddress& from, const OMacAddress& to, const OMacAddress& via );

    void identify( const OMacAddress&, const QString& ipaddr );

    void contextMenuRequested( QListViewItem* item, const QPoint&, int );

  signals:
    void rightButtonClicked(QListViewItem*,const QPoint&,int);
    void joinNetwork( const QString&, const QString&, int, const QString& );

  protected:
    void addIfNotExisting( MScanListItem* parent, const OMacAddress& addr, const QString& type = "station" );

};

//****************************** MScanListItem ****************************************************************

class MScanListItem: public OListViewItem
{
  public:
    MScanListItem::MScanListItem( QListView* parent,
                                  QString type = "unknown",
                                  QString essid = "unknown",
                                  QString macaddr = "unknown",
                                  bool wep = false,
                                  int channel = 0,
                                  int signal = 0 );

    MScanListItem::MScanListItem( QListViewItem* parent,
                                  QString type = "unknown",
                                  QString essid = "unknown",
                                  QString macaddr = "unknown",
                                  bool wep = false,
                                  int channel = 0,
                                  int signal = 0 );


  protected:
    virtual void decorateItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal );

  public:
    QString type;

  public:
    //const QString& type() { return _type; };
    const QString& essid() const;
    const QString& macaddr() { return _macaddr; };
    bool wep() { return _wep; };
    int channel() { return _channel; };
    int signal() { return _signal; };
    int beacons() { return _beacons; };

    void setSignal( int signal ) { /* TODO */ };
    void receivedBeacon();

    void setManufacturer( const QString& manufacturer );
    void setLocation( const QString& location );

    virtual OListViewItem* childFactory();
    virtual void serializeTo( QDataStream& s ) const;
    virtual void serializeFrom( QDataStream& s );

  protected:
    void playSound( const QString& ) const;

  private:
    QString _type;
    QString _essid;
    QString _macaddr;
    bool _wep;
    int _channel;
    int _signal;
    int _beacons;

};

//****************************** MScanListViewFactory ****************************************************************

/*

class MScanListViewFactory : public OListViewFactory
{
public:
    virtual QListView* listViewFactory();
    virtual QListViewItem* listViewItemFactory( QListView* lv );
    virtual QListViewItem* listViewItemFactory( QListViewItem* lvi );
    virtual void setColumnText( int depth, QListViewItem* lvi, int column, const QString& text );
    virtual void setCustomData( int depth, QListViewItem* lvi, const QString& text );
}
*/

#endif

