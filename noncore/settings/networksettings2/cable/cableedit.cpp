#include <GUIUtils.h>
#include <stdio.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include "cabledata.h"
#include "cableedit.h"

static long SpeedTable[] = { 
        2400, 9600, 19200, 38400, 57600, 115200, 
        230400, 460800, 500000, 921600, 1000000, 
        1152000, 1500000, 2000000, 2500000, 3000000, 
        3500000, 4000000, -1
};

CableEdit::CableEdit( QWidget * Parent ) : CableGUI( Parent ) {
}

void CableEdit::showData( CableData_t & D ) {
      DeviceFile_LE->setText( D.Device );
      LockFile_LE->setText( D.LockFile );
      for( long * Run = SpeedTable; *Run != -1; *Run ++ ) {
        if( *Run == D.Speed ) {
          Speed_CB->setCurrentItem( 
                ((long)(Run)-(long)(SpeedTable))/sizeof(long) );
          break;
        }
      }
      Parity_CB->setCurrentItem( D.Parity );
      DataBits_SB->setValue( D.DataBits );
      StopBits_CB->setCurrentItem( D.StopBits );
      HardwareFC_CB->setChecked( D.HardwareControl );
      SoftwareFC_CB->setChecked( D.SoftwareControl );
}

QString CableEdit::acceptable( void ) {
    if( DeviceFile_LE->text().isEmpty() )
      return tr("Device filename needed");
    return QString();
}

bool CableEdit::commit( CableData_t & D ) {
      bool SM = 0;

      TXTM( D.Device, DeviceFile_LE, SM );
      TXTM( D.LockFile, LockFile_LE, SM );

      for( long * Run = SpeedTable; *Run != -1; *Run ++ ) {
        if( *Run == D.Speed ) {
          if( D.Speed != Speed_CB->currentText().toLong() ) {
            SM = 1;
            D.Speed = Speed_CB->currentText().toLong();
          }
          break;
        }
      }

      CIM(D.Parity, Parity_CB, SM );

      SBM(D.DataBits, DataBits_SB, SM );
      CIM(D.StopBits, StopBits_CB, SM );

      CBM( D.HardwareControl, HardwareFC_CB, SM );
      CBM( D.SoftwareControl, SoftwareFC_CB, SM );

      return SM;
}
