#ifndef ROHFEEDBACK_H
#define ROHFEEDBACK_H

#ifdef OPIE_WITHROHFEEDBACK

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
#endif
