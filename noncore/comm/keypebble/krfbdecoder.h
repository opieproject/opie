// -*- c++ -*-

#ifndef KRFBDECODER_H
#define KRFBDECODER_H

#include <qobject.h>

class KRFBConnection;
class KRFBServerInfo;
class KRFBPixelFormat;
class KRFBBuffer;


typedef unsigned char CARD8;
typedef unsigned short CARD16;
typedef unsigned long CARD32;

/**
 * Negotiates the pixel format to be used then decodes the resulting
 * data stream.
 *
 * @author Richard Moore, rich@kde.org
 */
class KRFBDecoder : public QObject
{
  Q_OBJECT

public:
  friend class KRFBBuffer;

  enum State {
    AwaitingServerInit,
    AwaitingDesktopName,
    AwaitingUpdate,
    AwaitingRectHeader,
    AwaitingRawRectChunk,
    AwaitingCopyRectPos,
    AwaitingServerCutLength,
    AwaitingServerCutText,
    Idle,
    Error
  };

  /**
   * Create a KRFBDecoder that reads data from a logged in KRFBConnection
   * and sends its output to a KRFBBuffer.
   */
  KRFBDecoder( KRFBConnection *con );
  ~KRFBDecoder();

  void setBuffer( KRFBBuffer *buf ) { this->buf = buf; };
  void start();

  int toKeySym( QKeyEvent *k );

  //
  // Client -> Server messages
  //
  void sendUpdateRequest( bool incremental );
  void sendMouseEvent( QMouseEvent *e );
  void sendKeyPressEvent( QKeyEvent *e );
  void sendKeyReleaseEvent( QKeyEvent *e );
  void sendCutEvent( const QString &text );

protected:
  //
  // Initial format negotiation
  //
  void decidePixelFormat();
  void sendPixelFormat();
  void sendClientInit();
  void sendAllowedEncodings();

  //
  // Rectange processing
  //
  void handleRawRect();
  void handleCopyRect();
  void handleRRERect();
  void handleCoRRERect();
  void handleHexTileRect();

  void getRawRectChunk( int lines );

protected slots:
  void gotServerInit();
  void gotDesktopName();
  void gotUpdateHeader();
  void gotRectHeader();
  void gotRawRectChunk();
  void gotCopyRectPos();
  void gotServerCut();
  void gotServerCutLength();
  void gotServerCutText();
  void gotBell();

signals:
  void error( const QString & );
  void status( const QString & );

private:
  State currentState;

  // Used to store the state we were in before a cut or bell msg
  State oldState;

  // The number of rects we're expecting
  CARD16 noRects;

  //
  // Info about the current rect.
  //
  CARD16 x, y, w, h;
  int lines;
  CARD32 encoding;

  CARD32 serverCutTextLen;

  /** Where we draw the data (and the source of our events). */
  KRFBBuffer *buf;
  /** The connection to the server. */
  KRFBConnection *con;

  /** Info about the RFB server. */
  KRFBServerInfo *info;
  /** The pixel format we want. */
  KRFBPixelFormat *format;

  CARD8 buttonMask;
};

#endif // KRFBDECODER_H


