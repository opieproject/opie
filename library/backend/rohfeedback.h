/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef ROHFEEDBACK_H
#define ROHFEEDBACK_H


/*

    RightOnHold feedback show

*/

#define NOOFICONS 5

#include <qlabel.h>
#include <qtimer.h>

class QEvent;
class QPixmap;
class QBitmap;
class QMouseEvent;

namespace Opie {
namespace Internal {

class RoHFeedback : public QLabel {

      Q_OBJECT

      enum Actions {
        FeedbackStopped    = -2,
	FeedbackTimerStart = -1,
	FeedbackShow       =  0
      };
public :

      RoHFeedback();
      ~RoHFeedback();


      void init( const QPoint & P, QWidget* wid );
      void stop( void );
      int delay( void );

public slots :

      void iconShow( void );

protected :

      bool event( QEvent * E );

      QTimer Timer;
      int IconNr;
      QWidget * Receiver;

      static int IconWidth;
      static int IconHeight;
      static QPixmap * Imgs[NOOFICONS];
      static QBitmap * Masks[NOOFICONS];
};
}
}

#endif
