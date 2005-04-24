// coptright Mon 10-21-2002 01:14:03 by L. Potter <ljp@llornkcor.com>

#include "restart.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>
#include <qpe/qcopenvelope_qws.h>

RestartApplet::RestartApplet ( )
  : QObject ( 0, "RestartApplet" )
{
}

RestartApplet::~RestartApplet ( )
{
}

int RestartApplet::position ( ) const
{
    return 4;
}

QString RestartApplet::name ( ) const
{
  return tr( "Restart" );
}

QString RestartApplet::text ( ) const
{
  return tr( "Restart Opie" );
}

QIconSet RestartApplet::icon ( ) const
{
    QPixmap pix = Opie::Core::OResource::loadPixmap( "exec", Opie::Core::OResource::SmallIcon );
    return pix;
}

QPopupMenu *RestartApplet::popup ( QWidget * ) const
{
  return 0;
}

void RestartApplet::activated ( )
{
    QCopEnvelope e("QPE/System", "restart()");
}


QRESULT RestartApplet::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
  *iface = 0;
  if ( uuid == IID_QUnknown )
    *iface = this;
  else if ( uuid == IID_MenuApplet )
    *iface = this;
  else
    return QS_FALSE;

  if ( *iface )
    (*iface)-> addRef ( );
  return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
  Q_CREATE_INSTANCE( RestartApplet )
}


