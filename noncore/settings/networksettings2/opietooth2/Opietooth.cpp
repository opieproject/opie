#include <opie2/odebug.h>
#include <opie2/oledbox.h>
#include <opie2/ofiledialog.h>

using namespace Opie::Core;
using namespace Opie::Ui;

#include <qpe/resource.h>
#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdir.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmessagebox.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qtextstream.h>
#include <qtextview.h>

#include <Opietooth.h>
#include <OTDriver.h>
#include <OTPeer.h>
#include <OTGateway.h>
#include <OTSDPAttribute.h>
#include <OTSDPService.h>
#include <OTInquiry.h>

#include <system.h>

using namespace Opietooth2;

namespace Opietooth2 {

class PeerLVI : public QListViewItem {

public :

      PeerLVI( OTPeer * P, QListView * it ) : QListViewItem (it) {
        Peer = P;
      }
      ~PeerLVI( void ) {
      }

      inline OTPeer * peer( void )
        { return Peer; }

private :

      OTPeer * Peer;
};

class ChannelLVI : public QListViewItem {

public :

      ChannelLVI( int Ch, QListViewItem * it ) : QListViewItem (it) {
        Channel = Ch;
      }
      ~ChannelLVI( void ) {
      }

      inline int channel( void )
        { return Channel; }

private :

      int Channel;
};

class DriverLVI : public QListViewItem {

public :

      DriverLVI( OTDriver * P, QListView * it ) : QListViewItem (it) {
        Driver = P;
      }
      ~DriverLVI( void ) {
      }

      inline OTDriver * driver( void )
        { return Driver; }

private :

      OTDriver * Driver;
};

class LinkKeyLVI : public QListViewItem {

public :

      LinkKeyLVI( int Ch, QListView * it ) : QListViewItem (it) {
        LinkKey = Ch;
      }
      ~LinkKeyLVI( void ) {
      }

      inline int index( void )
        { return LinkKey; }

private :

      int LinkKey;
};
};

//
//
//
//
//

OTSniffing::OTSniffing( QWidget * parent ) : OTSniffGUI( parent ) {

      OT = OTGateway::getOTGateway();
      HciDump = 0;
}

OTSniffing::~OTSniffing() {
      SLOT_Trace( 0 );
}

void OTSniffing::SLOT_Trace( bool Run ) {

      if( ! Run ) {
        if ( HciDump ) {
          HciDump->process().kill();
          delete HciDump;
        }
        HciDump = 0;
        return;
      }

      HciDump = new MyProcess();
      QStringList SL;

      SL << "/usr/sbin/hcidump";
      switch( DataFormat_CB->currentItem() ) {
        case 0 : // Hex
          SL << "-x";
          break;
        case 1 : // Ascii
          SL << "-a";
          break;
        case 2 : // both
          SL << "-X";
          break;
      }

      SL << "-i";
      SL << OT->scanWith()->devname();

      connect( HciDump, 
               SIGNAL( stdoutLine( const QString & ) ),
               this, 
               SLOT( SLOT_Show( const QString & ) ) );

      connect( HciDump, 
               SIGNAL(processExited(MyProcess*) ),
               this, 
               SLOT( SLOT_ProcessExited(MyProcess*) ) );

      HciDump->process() << SL;

      if( ! HciDump->process().start( OProcess::DontCare, 
                                      OProcess::AllOutput )
        ) {
        QMessageBox::warning(0,
            tr("Run hcidump"),
            tr("Cannot start %1").arg(SL.join(" "))
        );
        delete HciDump;
        HciDump = 0;
      }

}

void OTSniffing::SLOT_Show( const QString & S ) {
      printf( "%s\n", S.latin1() );
      Output_TV->setText( Output_TV->text() + S + "\n" );

      QScrollBar *scroll = Output_TV->verticalScrollBar();
      scroll->setValue(scroll->maxValue());
      //Output_LB->insertItem( S );
      //Output_LB->setCurrentItem( Output_LB->count()-1 );
      //Output_LB->ensureCurrentVisible();
}

void OTSniffing::SLOT_ProcessExited( MyProcess * ) {
      printf( "Exited\n" );
      delete HciDump;
      HciDump = 0;
}

void OTSniffing::SLOT_Save( void ) {
      QString S = OFileDialog::getSaveFileName(
              OFileSelector::Extended,
              QDir::home().path(),
              QString::null,
              MimeTypes(),
              this );

      if( ! S.isEmpty() ) {
        QFile F( S );
        if( ! F.open( IO_WriteOnly ) ) {
          QMessageBox::warning(0,
              tr("Save log"),
              tr("Cannot open %1").arg(S) 
          );
          return;
        }
        QTextStream TS( &F );
        TS << S << endl;
      }
}

void OTSniffing::SLOT_Load( void ) {
      QString S = OFileDialog::getOpenFileName(
              OFileSelector::Extended,
              QDir::home().path(),
              QString::null,
              MimeTypes(),
              this );

      if( ! S.isEmpty() ) {
        QFile F( S );
        if( ! F.open( IO_ReadOnly ) ) {
          QMessageBox::warning(0,
              tr("Save log"),
              tr("Cannot open %1").arg(S)
          );
          return;
        }
        QTextStream TS ( &F );
        SLOT_ClearLog();
        S = TS.read(); 
        // Output_LB->insertStringList( QStringList::split( "\n", S ) );
        Output_TV->setText( S );
      }
}

void OTSniffing::SLOT_ClearLog( void ) {
      // Output_LB->clear();
      Output_TV->setText( "" );
}

//
//
//
//
//

OTPairing::OTPairing( QWidget * parent, OTIcons * _IC ) :
                OTPairingGUI( parent ) {

      OT = OTGateway::getOTGateway();
      Icons = (_IC ) ? _IC : new OTIcons();
      MyIcons = (_IC == 0 );

      // unpairing can only be done if bluetooth is disabled
      Unpair_But->setEnabled( ! OT->isEnabled() );
      if( ! OT->isEnabled() ) {
        Unpair_LBL->hide();
      } else {
        Unpair_LBL->show();
      }

      // open linkkey file and load pairs
      LinkKeyArray Keys = OT->getLinkKeys();
      LinkKeyLVI * it;
      OTPeer * P;
      OTDriver * D;

      for( unsigned int i = 0 ;
           i < Keys.count();
           i ++ ) {

        it = new LinkKeyLVI( i, Pairs_LV );

        P = 0;
        D = OT->findDriver( Keys[i].from() );

        if( D ) {
          it->setText( 0, D->devname() );

          // we are source
          P = OT->findPeer( Keys[i].to() );

          if( P ) {
            // put name
            it->setText( 1, P->name() );
          } else {
            // unknown
            it->setText( 1, Keys[i].to().toString() );
          }

          // and put address as sub
          QListViewItem * Sub = new QListViewItem( it );
          Sub->setText( 0, D->address().toString() );
          Sub->setText( 1, Keys[i].to().toString() );
        } else {
          // perhaps we are destination
          D = OT->findDriver( Keys[i].to() );

          if( D ) {
            it->setText( 1, D->devname() );

            // we are source
            P = OT->findPeer( Keys[i].from() );

            if( P ) {
              // put name
              it->setText( 0, P->name() );
            } else {
              // unknown
              it->setText( 0, Keys[i].from().toString() );
            }

            // and put address as sub
            QListViewItem * Sub = new QListViewItem( it );
            Sub->setText( 0, Keys[i].from().toString() );
            Sub->setText( 1, D->address().toString() );
          } else {
            // nor source nor destination -> unknown
            it->setText( 0, Keys[i].from().toString() );
            it->setText( 1, Keys[i].to().toString() );
          }
        }
      }
}


OTPairing::~OTPairing() {
      if( MyIcons )
        delete Icons;
      OTGateway::releaseOTGateway();
}

void OTPairing::SLOT_Unpair( ) {
      // find selected pair

      QListViewItem * it = Pairs_LV->firstChild();
      while( it ) {
        if( it->isSelected() ) {
          // confirm ?
          if( QMessageBox::warning(0,
              tr("Break pairing"),
              tr("Sure ?"),
              tr("Yes, break"),
              tr("No, don't break") ) == 0 ) {
            LinkKeyLVI * KPIt = (LinkKeyLVI *)it;
            // break 
            OT->removeLinkKey( KPIt->index() );
            delete KPIt;
          }
          return;
        }
        it= it->nextSibling();
      }
}

//
//
//
//
//

OTScan::OTScan( QWidget * parent, OTIcons * _IC ) :
                OTScanGUI( parent ), Filter() {

      OT = OTGateway::getOTGateway();
      Icons = (_IC ) ? _IC : new OTIcons();
      MyIcons = (_IC == 0 );
      DetectedPeers_LV->header()->hide();
      Current = 0;
      SelectedPeer = 0;
      SelectedChannel = 0;

      StrengthTimer = new QTimer( this );
      connect( StrengthTimer,
               SIGNAL( timeout()),
               this,
               SLOT( SLOT_UpdateStrength())
             );

      connect( OT,
               SIGNAL( detectedPeer( OTPeer *, bool )),
               this,
               SLOT( SLOT_NewPeer( OTPeer *, bool ))
             );
      connect( OT,
               SIGNAL( finishedDetecting()),
               this,
               SLOT( SLOT_FinishedDetecting())
             );

      // populate with peers we already know about
      const PeerVector & P = OT->peers();
      for( unsigned int i = 0;
           i < P.count();
           i ++ ) {
        SLOT_NewPeer( P[i], TRUE );
      }

      // populate State fram
      { QHBoxLayout * H =new QHBoxLayout( State_Frm );

        Paired_Led = new OLedBox( green, State_Frm );
        QLabel * L1 = new QLabel( tr( "Paired" ), State_Frm );

        H->addWidget( Paired_Led );
        H->addWidget( L1 );
        H->addStretch( 1 );
      }
}

OTScan::~OTScan() {
      if( MyIcons )
        delete Icons;
      OTGateway::releaseOTGateway();

      // send all peers that we do not care about states
      QListViewItem * Lit = DetectedPeers_LV->firstChild();
      while( Lit ) {
        ((PeerLVI *)Lit)->peer()->stopFindingOutState( );
        Lit = Lit->nextSibling();
      }
}

// static scan dialog function
int OTScan::getDevice( OTPeer *& Peer, 
                       int & Channel, 
                       OTGateway * OT,
                       const UUIDVector & Filter,
                       QWidget* Parent ) {
      bool IsUp = 0;
      unsigned int i;

      // check if bluetooth is up
      OTDriverList & DL = OT->getDriverList();
      for( i = 0;
           i < DL.count();
           i ++ ) {
        if( DL[i]->isUp() ) {
          // one device that is up found
          IsUp = 1;
          break;
        }
      }

      // use this driver
      OT->setScanWith( OT->driver(i) );

      // create dialog
      QDialog * Dlg = new QDialog( Parent, 0, TRUE );
      QVBoxLayout * V = new QVBoxLayout( Dlg );
      OTScan * Scn = new OTScan( Dlg );

      connect( Scn,
               SIGNAL( selected() ),
               Dlg,
               SLOT( accept() )
             );

      if( Filter ) {
        Scn->setScanFilter( Filter );
      }

      V->addWidget( Scn );
      Dlg->setCaption( tr("Scan Neighbourhood" ) );
      Dlg->showMaximized();
      int rv = Dlg->exec();

      if( rv == QDialog::Accepted ) {
        // get peer
        Peer = Scn->selectedPeer();
        if( Peer == 0 ) {
          // no peer selected
          rv = QDialog::Rejected;
        } else {
          Channel = Scn->selectedChannel();
        }
      }

      delete Dlg;

      return rv;
}

void OTScan::setScanFilter( const UUIDVector & V ) {
      Filter = V;
}

void OTScan::resetScanFilter( void ) {
      Filter.truncate(0);
}

void OTScan::SLOT_DoScan( bool DoIt ) {
      if( DoIt ) {
        OT->scanNeighbourhood();
      } else {
        OT->stopScanOfNeighbourhood();
      }

      scanMode( DoIt );
}

// double clicked on a device
void OTScan::SLOT_Selected( QListViewItem * it ) {
      if( ! it )
        return;

      if( Filter.count() > 0 ) {
        // filter on service 
        if( it->depth() == 0 ) {
          // select a service and not a device
          return;
        }

        // store result
        SelectedPeer = ((PeerLVI *)it->parent())->peer();
        SelectedChannel = ((ChannelLVI *)it)->channel();
      } else {
        // click on device
        if( it->depth() != 0 ) {
          return;
        }

        SelectedPeer = ((PeerLVI *)it)->peer();
        SelectedChannel = 0;
      }
      odebug << "Selected " << SelectedPeer->address().toString() <<
            " Channel " << SelectedChannel << oendl;
      emit selected();
}

void OTScan::SLOT_FinishedDetecting( ) {
      scanMode( false );
}

void OTScan::SLOT_CleanupOld( ) {

      // iterate over all peers and find those that
      // are down and have no pairing info
      OTPeer * TheP;
      const LinkKeyArray & Keys = OT->getLinkKeys();

      QListViewItem * Lit = DetectedPeers_LV->firstChild();
      while( Lit ) {
        TheP = ((PeerLVI *)Lit)->peer();
        if( TheP->state() == OTPeer::Peer_Down ) {
          unsigned int k;

          // what about linkkeys ?
          for( k = 0; k < Keys.count(); k ++ ) {
            if( TheP->address() == Keys[k].to() ||
                TheP->address() == Keys[k].from() 
              ) {
              // part of linkkey
              odebug << "LINKKEY " << TheP->address().toString() << oendl;
              break;
            }
          }

          if( k == Keys.count() ) {
            odebug << "RM LINKKEY " << TheP->address().toString() << oendl;
            // not found -> remember to remove this peer
            QListViewItem * Nit;
            OT->removePeer( TheP );
            Nit = Lit->nextSibling(); 
            delete Lit;
            Lit = Nit;
            continue;
          }
        } else {
          odebug << "NODOWN " << TheP->address().toString() << oendl;
        }

        Lit = Lit->nextSibling();
      }
}

void OTScan::SLOT_NewPeer( OTPeer * P, bool IsNew ){
      PeerLVI * it = 0;

      if( IsNew ) {
        it = new PeerLVI( P, DetectedPeers_LV );
      } else {
        // find peer in table
        QListViewItem * Lit = DetectedPeers_LV->firstChild();
        while( Lit ) {
          if( ((PeerLVI *)Lit)->peer() == P ) {
            // this item
            it = (PeerLVI *)Lit;
            break;
          }
          Lit = Lit->nextSibling();
        }

        if( ! it ) {
          odebug << "Should not occur" << oendl;
          return;
        }
      }

      // update/show info
      it->setText( 0, P->name() );
      it->setPixmap(0, Icons->deviceIcon( 
          OT->deviceTypeToName( P->deviceClass() ) ) );

      // tell peer to report its state async
      connect( P, 
               SIGNAL( peerStateReport( OTPeer *)),
               this,
               SLOT( SLOT_PeerState( OTPeer *))
             );

      if( IsNew ) {
        // find state
        refreshState( (PeerLVI *)it, 1 );
      } else {
        // update staet
        SLOT_PeerState( P );
      }
}

void OTScan::SLOT_PeerState( OTPeer * P ) {
      PeerLVI * it = (PeerLVI *)DetectedPeers_LV->firstChild();
      while( it ) {
        if( it->peer() == P ) {
          break;
        }
        it = (PeerLVI * )it->nextSibling();
      }

      if( ! it )
        return;

      switch( P->state() ) {
        case OTPeer::Peer_Unknown :
        case OTPeer::Peer_Down :
        it->setPixmap( 1, 0 );
          break;
        case OTPeer::Peer_Up :
          it->setPixmap( 1, Icons->loadPixmap( 
            ( P->connectedTo() ) ?  "connected" : "notconnected" ) );
          if( it == Current && ! StrengthTimer->isActive() ) {
            // start showing strength
            StrengthTimer->start( 1000, FALSE );
            SLOT_UpdateStrength();
          }
          break;
      }
}

void OTScan::SLOT_RefreshState( void ) {

      QListViewItem * it = DetectedPeers_LV->firstChild();
      while( it ) {
        if( it->isSelected() ) {
          break;
        }
        it = it->nextSibling();
      }

      if( ! it )
        return;

      refreshState( (PeerLVI *)it, 1 );
}

void OTScan::refreshState( PeerLVI * it, bool Force ) {
      it->setPixmap( 1, Icons->loadPixmap( "find" ) );
      it->peer()->findOutState( 30, Force );
}

void OTScan::SLOT_Show( QListViewItem * it ) {

      if( ! it || it->depth() > 0 )
        return;

      QString S;

      Current = (PeerLVI *)it;

      Strength_PB->setProgress( 0 ); // reset
      Address_LBL->setText( Current->peer()->address().toString() );
      Peer_GB->setTitle( Current->peer()->name() );

      const LinkKeyArray & Keys = OT->getLinkKeys();

      Paired_Led->setOn( FALSE );
      for( unsigned int i = 0;
           i < Keys.count();
           i ++ ) {
        if( Current->peer()->address() == Keys[i].to() ) {
          Paired_Led->setOn( TRUE );
          break;
        }
      }

      if( Current->peer()->state() == OTPeer::Peer_Up ) {
        RefreshServices_But->setEnabled( TRUE );
        StrengthTimer->start( 1000, FALSE );
        SLOT_UpdateStrength();
      } else {
        RefreshServices_But->setEnabled( FALSE );
      }

}

void OTScan::SLOT_UpdateStrength( void ) {
      OTDriver * D = Current->peer()->connectedTo();

      if( D ) {
        long Q = D->getLinkQuality( Current->peer()->address() );
        Strength_PB->setProgress( Q );
        if( ! Q ) {
          // no quality 
          Strength_PB->setEnabled( TRUE );
          StrengthTimer->stop();
        }
      } else {
        Strength_PB->setEnabled( FALSE );
        Strength_PB->setProgress( 0 );
        // no point in continuing
        StrengthTimer->stop();
      }
}

void OTScan::SLOT_RefreshServices( void ) {

      QListViewItem * it = DetectedPeers_LV->firstChild();
      while( it ) {
        if( it->isSelected() ) {
          break;
        }
        it = it->nextSibling();
      }

      if( ! it )
        return;

      QString S;
      PeerLVI * PI = (PeerLVI *)it;

      scanMode( true );
      qApp->processEvents(0);

      ServiceVector & V = PI->peer()->services();

      while( PI->firstChild() ) {
        // remove children
        delete PI->firstChild();
      }

      for( unsigned int i = 0 ; 
           i < V.count();
           i ++ ) {
        QString S;
        S = V[i]->name();

        if( S.isEmpty() ) {
          continue;
        }

        { QListViewItem * SIt;
          UUIDVector UIDV; 
          QPixmap Pm;
          bool Done = 0;
          bool R;
          short ID;

          SIt = 0;

          UIDV = V[i]->classIDList();
          // first all UUID ! 1200 12ff (Genericprofiles)
          for( unsigned int j = 0;
               j < UIDV.count();
               j ++ ) {

            if( Filter.count() ) {
              bool FilterOut = 1;
              // filter out if not in list
              for( unsigned int ff = 0;
                   ff < Filter.count();
                   ff ++ ) {
                if( UIDV[j] == Filter[ff] ) {
                  FilterOut = 0;
                  break;
                }
              }

              if( FilterOut ) {
                // not in filter list
                continue;
              }
            } // else show

            ID = UIDV[j].toShort();
            if( ID < 0x1200 || ID > 0x12ff ) {
              // use this profile
              if( R ) {
                unsigned int ch;
                bool has;
                has = V[i]->rfcommChannel( ch );
                SIt = new ChannelLVI( (has) ? (int)ch : -1 , PI );
                SIt->setText(0, V[i]->name() );

                Pm = Icons->serviceIcon( ID, R );
                SIt->setPixmap(0, Pm );
                Done = 1;
                break;
              }
            }
          }

          if( ! Done ) {
            // check other range too
            for( unsigned int j = 0;
                 j < UIDV.count();
                 j ++ ) {

              if( Filter.count() ) {
                bool FilterOut = 1;
                // filter out if not in list
                for( unsigned int ff = 0;
                     ff < Filter.count();
                     ff ++ ) {
                  if( UIDV[j] == Filter[ff] ) {
                    FilterOut = 0;
                    break;
                  }
                }

                if( FilterOut ) {
                  // not in filter list
                  continue;
                }
              } // else show

              ID = UIDV[j].toShort();
              if( ID >= 0x1200 && ID <= 0x12ff ) {
                // use this profile
                unsigned int ch;
                bool has;
                has = V[i]->rfcommChannel( ch );
                SIt = new ChannelLVI( (has) ? (int)ch : -1 , PI );
                SIt->setText(0, V[i]->name() );

                Pm = Icons->serviceIcon( ID, R );
                SIt->setPixmap(0, Pm );

                break;
              }
            }
          }

        }
      }

      scanMode( false );
}

void OTScan::scanMode( bool M ) {
      // avoid infinite loop because it triggers DoScan
      Detect_But->blockSignals( TRUE );
      Detect_But->setOn( M );
      Detect_But->setText( (M) ? tr("Scanning") : tr("Scan") );
      Detect_But->blockSignals( FALSE );
}

//
//
//
//
//

OTManage::OTManage( QWidget * parent, OTIcons * _IC ) :
            OTManageGUI( parent ) {

      OT = OTGateway::getOTGateway();

      Icons = (_IC ) ? _IC : new OTIcons();
      MyIcons = (_IC == 0 );
      AllDrivers_LV->setSorting(-1);

      connect( OT, 
               SIGNAL( driverListChanged() ),
               this,
               SLOT( SLOT_DriverListChanged() ) 
             );
      connect( OT,
               SIGNAL( stateChange( OTDriver *, bool ) ),
               this,
               SLOT( SLOT_StateChange( OTDriver *, bool ) )
             );

      SLOT_DriverListChanged();

      AllDrivers_LV->header()->hide();
}

OTManage::~OTManage() {
      if( MyIcons )
        delete Icons;
      OTGateway::releaseOTGateway();
}

void OTManage::SLOT_ShowDriver( QListViewItem * It ) {
      if( It == 0 || It->depth() > 0 ) 
        // not toplevel
        return;

      DriverLVI * it = (DriverLVI *) It;
      DriverIsUp_CB->setChecked( it->driver()->isUp() );
}

void OTManage::SLOT_UpDriver( bool Up ) {
      QListViewItem * it = AllDrivers_LV->firstChild();
      while( it ) {
        if( it->isSelected() ) {
          OTDriver * D = ((DriverLVI *)it)->driver();
          odebug << "UP driver " << D->devname() << oendl;
          // this
          D->setUp( Up );
          return;
        }
        it = it->nextSibling();
      }
}

void OTManage::SLOT_StateChange( OTDriver * D, bool Up ) {
      QListViewItem * it = AllDrivers_LV->firstChild();
      while( it ) {
        if( ((DriverLVI *)it)->driver() == D ) {
          it->setPixmap( 0, 
               Icons->loadPixmap( ( Up ) ?  "bluezon" : "bluezoff" ) );
          return;
        }
        it = it->nextSibling();
      }
}

void OTManage::SLOT_DriverListChanged( ) {
      DriverLVI * It;
      QListViewItem * Sub;
      QListViewItem * First = 0;
      OTDriver* D;
      OTDriverList & DL = OT->getDriverList();

      AllDrivers_LV->clear();
      for( unsigned int i = 0;
           i < DL.count();
           i ++ ) {
        D = DL[i];
        It = new DriverLVI( D, AllDrivers_LV );

        if( ! First )
          First = It;

        It->setText( 0, D->devname() );
        It->setPixmap( 0, 
             Icons->loadPixmap( (D->isUp()) ? 
                                  "bluezon" : "bluezoff" ) );

        Sub = new QListViewItem( It );
        Sub->setText( 0, tr( "Name" ) );
        Sub->setText( 1, D->name() ); 

        Sub = new QListViewItem( It );
        Sub->setText( 0, tr( "Address" ) );
        Sub->setText( 1, D->address().toString() ); 

        Sub = new QListViewItem( It );
        Sub->setText( 0, tr( "Revision" ) );
        Sub->setText( 1, D->revision() ); 

        Sub = new QListViewItem( It );
        Sub->setText( 0, tr( "Manufacturer" ) );
        Sub->setText( 1, D->manufacturer() ); 

        QString Service, Device;
        D->getClass( Service, Device );

        Sub = new QListViewItem( It );
        Sub->setText( 0, tr( "Service classes" ) );
        Sub->setText( 1, Service ); 
        Sub = new QListViewItem( It );
        Sub->setText( 0, tr( "Device class" ) );
        Sub->setText( 1, Device ); 
      }

      if( DL.count() ) {
        AllDrivers_LV->setCurrentItem( First );
        DriverIsUp_CB->setEnabled( TRUE );
      } else {
        DriverIsUp_CB->setChecked( FALSE );
        DriverIsUp_CB->setEnabled( FALSE );
      }
}

void OTManage::SLOT_SetRefreshTimer( int v ) {
      OT->setRefreshTimer( v * 1000 );
}

//
//
//
//
//

OTMain::OTMain( QWidget * parent ) : OTMainGUI( parent ) {

      Icons = new OTIcons();
      SnifWindow = 0;
      OT = OTGateway::getOTGateway();

      connect( OT,
               SIGNAL( deviceEnabled( bool ) ),
               this,
               SLOT( SLOT_DeviceIsEnabled( bool ) )
             );
      connect( OT, 
               SIGNAL( driverListChanged() ),
               this,
               SLOT( SLOT_DriverListChanged() ) 
             );
      connect( OT,
               SIGNAL( stateChange( OTDriver *, bool ) ),
               this,
               SLOT( SLOT_StateChange( OTDriver *, bool ) )
             );

      if( ! OT->needsEnabling() ) {
        MustBeEnabled_CB->hide();
      } else {
        // detect current state
        MustBeEnabled_CB->setChecked(
            OT->isEnabled() );
      }

      SLOT_DriverListChanged();
}

OTMain::~OTMain() {
      OTGateway::releaseOTGateway();
      delete Icons;
}

void OTMain::SLOT_DriverListChanged() {
      OTDriver * D;
      OTDriverList & DL = OT->getDriverList();

      DeviceList_CB->clear();
      for( unsigned int i = 0;
           i < DL.count();
           i ++ ) {
        D = DL[i];
        DeviceList_CB->insertItem(
           Icons->loadPixmap( (D->isUp()) ? 
                              "bluezon" : "bluezoff" ),
           D->devname() );
        if( D == OT->scanWith() ) {
          DeviceList_CB->setCurrentItem( i );
        }
      }

      Scan_But->setEnabled( OT->getDriverList().count() > 0 );
      DeviceList_CB->setEnabled( OT->getDriverList().count() > 0 );
}

void OTMain::SLOT_EnableBluetooth( bool Up ) {
      OT->SLOT_SetEnabled( Up );
}

void OTMain::SLOT_DeviceIsEnabled( bool Up ) {
      MustBeEnabled_CB->blockSignals( TRUE );
      MustBeEnabled_CB->setChecked( Up );
      MustBeEnabled_CB->blockSignals( FALSE );
}

void OTMain::SLOT_Manage( void ) {
      QDialog * Dlg = new QDialog( this, 0, TRUE );
      QVBoxLayout * V = new QVBoxLayout( Dlg );
      OTManage * Mng = new OTManage( Dlg, Icons );

      V->addWidget( Mng );

      Dlg->setCaption( tr("Manage local devices" ) );
      Dlg->showMaximized();
      Dlg->exec();
      delete Dlg;
}

void OTMain::SLOT_Scan( void ) {
      OTDriverList & DL = OT->getDriverList();
      for( unsigned int i = 0;
           i < DL.count();
           i ++ ) {
        if( DL[i]->isUp() &&
            DL[i]->devname() == DeviceList_CB->currentText()
          ) {
          QDialog * Dlg = new QDialog( this, 0, TRUE );
          QVBoxLayout * V = new QVBoxLayout( Dlg );
          OTScan * Scn = new OTScan( Dlg, Icons );

          OT->setScanWith( OT->driver(i) );
          V->addWidget( Scn );
          Dlg->setCaption( tr("Scan Neighbourhood" ) );
          Dlg->showMaximized();
          Dlg->exec();

          delete Dlg;
          return;
        }
      }

}

void OTMain::SLOT_StateChange( OTDriver * D, bool Up ) {
      for( int i = 0;
           i < DeviceList_CB->count();
           i ++ ) {
        if( DeviceList_CB->text(i) == D->devname() ) {
          DeviceList_CB->changeItem(
             Icons->loadPixmap( (Up) ? "bluezon" : "bluezoff" ),
             D->devname(),
             i );
          return;
        }
      }
}

void OTMain::SLOT_Pairing( void ) {
      QDialog * Dlg = new QDialog( this, 0, TRUE );
      QVBoxLayout * V = new QVBoxLayout( Dlg );
      OTPairing * Pair = new OTPairing( Dlg, Icons );

      V->addWidget( Pair );
      Dlg->showMaximized();
      Dlg->setCaption( tr("Manage pairing" ) );
      Dlg->exec();

      delete Dlg;
}

void OTMain::SLOT_Sniffing( void ) {

      if( SnifWindow == 0 ) {
        SnifWindow = new QDialog( this, 0, FALSE );
        QVBoxLayout * V = new QVBoxLayout( SnifWindow );
        OTSniffing * SN = new OTSniffing( SnifWindow );
        V->addWidget( SN );
      }

      SnifWindow->showMaximized();
      SnifWindow->show();
}
