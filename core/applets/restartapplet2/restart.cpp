// coptright Mon 10-21-2002 01:14:03 by L. Potter <ljp@llornkcor.com>

#include <qpe/qpeapplication.h>
#include <qpe/resource.h>
#include <qpe/qcopenvelope_qws.h>

#include "restart.h"

// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/stat.h>
//#include <dirent.h>
/* XPM */
static char *restart_xpm[] = {
"16 16 11 1",
"   c None",
".  c #000000",
"+  c #DCDCDC",
"@  c #A0A0A0",
"#  c #C3C3C3",
"$  c #808080",
"%  c #FFA858",
"&  c #FFDCA8",
"*  c #FFFFC0",
"=  c #FFFFFF",
"-  c #585858",
"       ..       ",
"   .. .++. ..   ",
"  .+@.@##@.@+.  ",
"  .@+$@%%@$+@.  ",
"   .$%%&%&%$.   ",
" ..+@%&$$%&@+.. ",
".+#@%&%@@&*%@#+.",
".$@+$&*&&=*$+@$.",
" .--+$&*=&$+--. ",
"  .$#++$$++#$.  ",
" .@=$-$++$-$=@. ",
" .+@-..@@..-@+. ",
"  ... .+=. ...  ",
"      .-$.      ",
"       ..       ",
"                "};

RestartApplet::RestartApplet ( )
  : QObject ( 0, "RestartApplet" ), ref ( 0 )
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
  QPixmap pix;
  QImage img =  ( const char** ) restart_xpm ;//Resource::loadImage ( "Run" );
  
  if ( !img. isNull ( ))
    pix. convertFromImage ( img. smoothScale ( 14, 14 ));
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

  if ( *iface )
    (*iface)-> addRef ( );
  return QS_OK;
}

Q_EXPORT_INTERFACE( )
{
  Q_CREATE_INSTANCE( RestartApplet )
}


