#ifndef OPIETOOTH_H
#define OPIETOOTH_H

#include <OTIcons.h>

#include <OTPairingGUI.h>
namespace Opie { namespace Ui { class OLedBox; }; };

namespace Opietooth2 {

class OTGateway;
class OTDriver;
class OTInquiry;
class OTPeer;
class PeerLVI;

class OTPairing : public OTPairingGUI {

      Q_OBJECT

public :

      OTPairing( QWidget * parent, 
                 OTIcons * _Ic = 0 );
      ~OTPairing();

private slots :

      void SLOT_Unpair( void );

signals :

protected :

private :

      bool              MyIcons;
      OTIcons *         Icons;
      OTGateway *       OT;
};
};

#include <OTScanGUI.h>

namespace Opietooth2 {

class OTGateway;
class OTDriver;
class OTInquiry;
class OTPeer;

class OTScan : public OTScanGUI {

      Q_OBJECT

public :

      OTScan( QWidget * parent, 
              OTIcons * _Ic = 0 );
      ~OTScan();

      // static function to return a device and a channel
      static int getDevice( OTPeer *& Peer, 
                            int & Channel,
                            OTGateway * OT,
                            const UUIDVector & Filter = 0,
                            QWidget* Parent = 0);

      // show only services that match any of the filter
      void setScanFilter( const UUIDVector & Filter );
      void resetScanFilter( void );

      inline OTPeer * selectedPeer( void )
        { return SelectedPeer; }
      inline int selectedChannel( void )
        { return SelectedChannel; }

public slots :

private slots :

      void SLOT_DoScan( bool );
      void SLOT_NewPeer( OTPeer *, bool );
      void SLOT_FinishedDetecting();
      void SLOT_Show( QListViewItem *);
      void SLOT_RefreshServices( void );
      void SLOT_RefreshState( void );
      void SLOT_CleanupOld( void );
      void SLOT_UpdateStrength( void );
      void SLOT_PeerState( OTPeer * );
      void SLOT_Selected( QListViewItem * );

signals :

      void selected( void );

protected :

private :

      void refreshState( PeerLVI *, bool );
      void scanMode( bool );

      // load scanned devices

      bool              MyIcons;
      OTIcons *         Icons;
      OTGateway *       OT;
      OTInquiry *       Scanning;
      UUIDVector        Filter;

      Opie::Ui::OLedBox *         Paired_Led;
      QTimer *          StrengthTimer;
      PeerLVI *         Current;

      OTPeer *          SelectedPeer;
      int               SelectedChannel;
};
};

#include <OTManageGUI.h>
namespace Opietooth2 {

class OTManage : public OTManageGUI {

      Q_OBJECT

public :

      OTManage( QWidget * parent,
                OTIcons * _IC = 0 );
      ~OTManage();

public slots :

private slots :

      void SLOT_ShowDriver( QListViewItem * );
      void SLOT_UpDriver( bool );
      void SLOT_StateChange( OTDriver * , bool );
      void SLOT_DriverListChanged();
      void SLOT_SetRefreshTimer( int );

signals :

protected :

private :

      // load scanned devices

      bool              MyIcons;
      OTIcons *         Icons;
      OTGateway *       OT;
      OTInquiry *       Scanning;
};
};

#include <OTMainGUI.h>

namespace Opietooth2 {
class OTMain : public OTMainGUI {

      Q_OBJECT

public :

      OTMain( QWidget * parent );
      ~OTMain();

public slots :

private slots :

      void SLOT_Pairing( void );
      void SLOT_Manage( void );
      void SLOT_Scan( void );
      void SLOT_EnableBluetooth( bool );
      void SLOT_DriverListChanged();
      void SLOT_DeviceIsEnabled( bool );
      void SLOT_StateChange( OTDriver * , bool );

signals :

protected :

private :

      // load scanned devices
      OTIcons *         Icons;
      OTGateway *       OT;
};
};
#endif
