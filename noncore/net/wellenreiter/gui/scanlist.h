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

#include <opie2/olistview.h>

#include <qtextstream.h>

class QString;
class ManufacturerDB;

class MScanListView: public OListView
{
  Q_OBJECT

  public:
    MScanListView( QWidget* parent = 0, const char* name = 0 );
    virtual ~MScanListView();

    void setManufacturerDB( ManufacturerDB* manufacturerdb );

    virtual OListViewItem* childFactory();
    virtual void serializeTo( QDataStream& s ) const;
    virtual void serializeFrom( QDataStream& s );

  public slots:
    void addNewItem( QString type, QString essid, QString macaddr, bool wep, int channel, int signal );
    void traffic( QString type, QString from, QString to, QString via, QString additional = QString::null );

  private:
    ManufacturerDB* _manufacturerdb;

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
    const QString& essid() { return _essid; };
    const QString& macaddr() { return _macaddr; };
    bool wep() { return _wep; };
    int channel() { return _channel; };
    int signal() { return _signal; };
    int beacons() { return _beacons; };

    void setSignal( int signal ) { /* TODO */ };
    void receivedBeacon();

    void setManufacturer( const QString& manufacturer );

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

