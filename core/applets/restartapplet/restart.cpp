/**********************************************************************
** Copyright (C) 2002 L.J. Potter ljp@llornkcor.com
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

#include "restart.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdatastream.h>


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
RestartApplet::RestartApplet( QWidget *parent, const char *name )
    : QWidget( parent, name ) {
      setFixedHeight( 18 );
      setFixedWidth( 16 );
    qDebug("new restartapplet");
    show();
}

RestartApplet::~RestartApplet() {
}

void RestartApplet::mousePressEvent( QMouseEvent *) {
    QCopEnvelope e("QPE/System", "restart()");
}

void RestartApplet::paintEvent( QPaintEvent* ) {
      QPainter p(this);
      qDebug("paint RESTART pixmap");
      p.drawPixmap( 0, 1,  ( const char** ) restart_xpm );
}

