/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
**  All rights reserved.
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

#ifndef __WIRELESS_APPLET_H__
#define __WIRELESS_APPLET_H__

#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>

class MNetwork;
class MWirelessNetwork;
class MNetworkInterface;
class MWirelessNetworkInterface;
class Y;
class QLabel;
class WirelessApplet;
class MGraph;

class WirelessControl : public QFrame
{
    Q_OBJECT
public:
    WirelessControl( WirelessApplet* icon, QWidget *parent=0, const char *name=0 );
    void show( bool );
    
    void readConfig();
    void writeConfigEntry( const char* entry, int val );

    MGraph* mgraph;
    QLabel* statusLabel;
    QLabel* updateLabel;

public slots:
    void updateDelayChange( int );
    void displayStyleChange( int );
    void advancedConfigClicked();
        
private:
    WirelessApplet* applet;

    int displayStyle;
    int updateFrequency;

    bool rocESSID;
    bool rocFREQ;
    bool rocAP;
    bool rocMODE;
};

class WirelessApplet : public QWidget
{
    Q_OBJECT
public:
    WirelessApplet( QWidget *parent = 0, const char *name=0 );
    ~WirelessApplet();
    WirelessControl* status;
    
    virtual void timerEvent( QTimerEvent* );
    void updateDelayChange( int delay );
    void displayStyleChange( int style );
    
    void updateDHCPConfig( bool, bool, bool, bool );

private:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent* );
    void checkInterface();
    void renewDHCP();
    
    bool mustRepaint();
    void updatePopupWindow();
    const char** getQualityPixmap();

private:
    QPixmap snapshotPixmap;
    int visualStyle;
    int timer;
    
    MWirelessNetwork* network;
    MNetworkInterface* interface;
    
private:
    const char** oldpixmap;
    MWirelessNetworkInterface* oldiface;
    int oldqualityH;
    int oldsignalH;
    int oldnoiseH;       

    QString oldESSID;
    QString oldAP;
    QString oldMODE;
    double oldFREQ;
    
    bool rocESSID;
    bool rocFREQ;
    bool rocAP;
    bool rocMODE;
};

#endif // __WIRELESS_APPLET_H__

