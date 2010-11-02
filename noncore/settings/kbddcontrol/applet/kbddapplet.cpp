/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "kbddapplet.h"
#include "../kbddhandler.h"

/* Opie */
#include <opie2/oprocess.h>
#include <opie2/otaskbarapplet.h>
#include <qpe/applnk.h> // for AppLnk
#include <qpe/resource.h> // for Resource loading
#include <qpe/config.h>
#include <qtopia/qcopenvelope_qws.h>

/* Qt */
#include <qlabel.h>
#include <qpainter.h>
#include <qmessagebox.h>

using namespace Opie::Core;

KbddApplet::KbddApplet(QWidget *parent)
    : QWidget( parent,  tr("kbdd Applet") ) {
    QImage image = Resource::loadImage("kbddcontrol/kbddcontrol");
    image = image.smoothScale(AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    m_pix = new QPixmap();
    m_pix->convertFromImage( image );
    setFixedHeight(AppLnk::smallIconSize() );
    setFixedWidth( AppLnk::smallIconSize() );
}

KbddApplet::~KbddApplet() {
    delete m_pix;
}

void KbddApplet::mousePressEvent(QMouseEvent* ) {
    QPopupMenu * menu = new QPopupMenu( this );
    QStringList profiles;
    KbddHandler kbdd;

    kbdd.readConfig();
    const QString &currport = kbdd.getPort();
    const QString &currtype = kbdd.getType();

    Config cfg ("kbddcontrol");
    profiles = cfg.groupList();
    int i = 3;
    for (QStringList::Iterator it = profiles.begin(); it != profiles.end(); ++it ) {
        cfg.setGroup(*it);
        menu->insertItem( cfg.readEntry("name"), i );
        if(currport == cfg.readEntry("device") && currtype == cfg.readEntry("keybtype"))
            menu->setItemChecked(i, true);
        i++;
    }

    if( !profiles.isEmpty() )
        menu->insertSeparator();
    menu->insertItem( tr("Settings...") , 0 );
    menu->insertItem( tr("Restart") , 1 );
    menu->insertItem( tr("Disable") , 2 );

    /* Check if kbdd is running */
    int pid = OProcess::processPID("kbdd");
    if(pid==0)
        menu->setItemChecked(2, true);

    QPoint p = mapToGlobal( QPoint( 0, 0 ) );
    QSize s = menu->sizeHint();
    int opt = menu->exec( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                                  p.y() - s.height() ), 0 );
    if(opt > -1) {
        KbddHandler kbdd;
        if(opt == 0) {
            QCopEnvelope e ( "QPE/Application/kbddcontrol", "raise()" );
        }
        else if(opt == 1) {
            kbdd.restart();
        }
        else if(opt == 2) {
            kbdd.stop();
        }
        else {
            cfg.setGroup(profiles[opt - 2]);
            kbdd.setPort(cfg.readEntry("device"));
            kbdd.setType(cfg.readEntry("keybtype"));
            kbdd.writeConfig();
            kbdd.restart();
        }
    }

    updateStatus();

    delete menu;
}

void KbddApplet::paintEvent( QPaintEvent* ) {
    QPainter p(this);
    p.drawPixmap(0, 0,  *m_pix );
}

void KbddApplet::updateStatus(void) {

}

/*
 * We need to add this symbol for the plugin exporter!
 */
int KbddApplet::position(){
    return 1;
}


/*
 * Here comes the implementation of the interface
 */
EXPORT_OPIE_APPLET_v1( KbddApplet )

