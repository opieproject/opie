#include "krfbconnection.h"
#include "krfbserver.h"
#include "krfbserverinfo.h"
#include "krfbdecoder.h"
#include "krfbbuffer.h"

#include <qpe/qpeapplication.h>

#include <qpixmap.h>
#include <qsocket.h>
#include <qevent.h>
#include <qstring.h>
#include <qclipboard.h>

#include <assert.h>

//
// Endian stuff
//
#ifndef KDE_USE_FINAL
const int endianTest = 1;
#endif

#define Swap16IfLE(s) \
    (*(char *)&endianTest ? ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) : (s))

#define Swap32IfLE(l) \
    (*(char *)&endianTest ? ((((l) & 0xff000000) >> 24) | \
			     (((l) & 0x00ff0000) >> 8)  | \
			     (((l) & 0x0000ff00) << 8)  | \
			     (((l) & 0x000000ff) << 24))  : (l))

//
// The lengths of the messages we need to wait for
//
const int ServerInitLength = 24;
const int UpdateHeaderLength = 4;
const int RectHeaderLength = 12;
const int RectChunkSize = 4;
const int CopyRectPosLength = 4;
const int ServerCutLenLength = 7;

//
// Client -> Server Message Identifiers
//
static CARD8 SetPixelFormatId = 0;
//static CARD8 FixColourMapEntriesId = 1; // Not used
static CARD8 SetEncodingsId = 2;
static CARD8 UpdateRequestId = 3;
static CARD8 KeyEventId = 4;
static CARD8 PointerEventId = 5;
static CARD8 ClientCutTextId = 6;

//
// Server -> Client Message Identifiers
//
static CARD8 UpdateId = 0;
static CARD8 BellId = 2;
static CARD8 ServerCutId = 3;

//
// Encoding identifiers
//
static CARD32 RawEncoding = Swap32IfLE( 0 );
static CARD32 CopyRectEncoding = Swap32IfLE(1 );
static CARD32 RreEncoding = Swap32IfLE( 2 );
static CARD32 CorreEncoding = Swap32IfLE( 4 );
static CARD32 HexTileEncoding = Swap32IfLE( 5 );

static struct {
    int keysym;
    int keycode;
} keyMap[] = {
    { 0xff08, Qt::Key_Backspace },
    { 0xff09, Qt::Key_Tab       },
    { 0xff0d, Qt::Key_Return    },
    { 0xff1b, Qt::Key_Escape    },
    { 0xff63, Qt::Key_Insert    },
    { 0xffff, Qt::Key_Delete    },
    { 0xff50, Qt::Key_Home      },
    { 0xff57, Qt::Key_End       },
    { 0xff55, Qt::Key_Prior     },
    { 0xff56, Qt::Key_Next      },
    { 0xff51, Qt::Key_Left      },
    { 0xff52, Qt::Key_Up        },
    { 0xff53, Qt::Key_Right     },
    { 0xff54, Qt::Key_Down      },
    { 0xffbe, Qt::Key_F1        },
    { 0xffbf, Qt::Key_F2        },
    { 0xffc0, Qt::Key_F3        },
    { 0xffc1, Qt::Key_F4        },
    { 0xffc2, Qt::Key_F5        },
    { 0xffc3, Qt::Key_F6        },
    { 0xffc4, Qt::Key_F7        },
    { 0xffc5, Qt::Key_F8        },
    { 0xffc6, Qt::Key_F9        },
    { 0xffc7, Qt::Key_F10       },
    { 0xffc8, Qt::Key_F11       },
    { 0xffc9, Qt::Key_F12       },
    { 0xffe1, Qt::Key_Shift     },
    { 0xffe2, Qt::Key_Shift     },
    { 0xffe3, Qt::Key_Control   },
    { 0xffe4, Qt::Key_Control   },
    { 0xffe7, Qt::Key_Meta      },
    { 0xffe8, Qt::Key_Meta      },
    { 0xffe9, Qt::Key_Alt       },
    { 0xffea, Qt::Key_Alt       },
    { 0, 0 }
};


KRFBDecoder::KRFBDecoder( KRFBConnection *con )
  : QObject( con, "RFB Decoder" )
{
  assert( con );
  assert( con->state() == KRFBConnection::Connected );

  this->con = con;
  this->buf = 0;
  this->info = 0;
  this->format = 0;
  this->buttonMask = 0;
  currentState = Idle;
}

KRFBDecoder::~KRFBDecoder()
{
  if ( info )
    delete info;
  if ( format )
    delete format;
}

void KRFBDecoder::start()
{
  sendClientInit();
}

void KRFBDecoder::sendClientInit()
{
  con->write( &( con->options()->shared ), 1 );

  // Wait for server init
  qWarning( "Waiting for server init" );

  static QString statusMsg = tr( "Waiting for server initialisation..." );
  emit status( statusMsg );

  currentState = AwaitingServerInit;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotServerInit() ) );
  con->waitForData( ServerInitLength );
}

void KRFBDecoder::gotServerInit()
{
  qWarning( "Got server init" );
  disconnect( con, SIGNAL( gotEnoughData() ), this, SLOT( gotServerInit() ) );

  if ( info )
    delete info;
  info = new KRFBServerInfo;
  CHECK_PTR( info );

  con->read( &(info->width), 2 );
  info->width = Swap16IfLE( info->width );
  con->read( &info->height, 2 );
  info->height = Swap16IfLE( info->height );

  con->read( &(info->bpp), 1 );
  con->read( &(info->depth), 1 );
  con->read( &(info->bigEndian), 1 );
  con->read( &(info->trueColor), 1 );

  con->read( &(info->redMax), 2 );
  info->redMax = Swap16IfLE( info->redMax );
  con->read( &(info->greenMax), 2 );
  info->greenMax = Swap16IfLE( info->greenMax );
  con->read( &(info->blueMax), 2 );
  info->blueMax = Swap16IfLE( info->blueMax );

  con->read( &(info->redShift), 1 );
  con->read( &(info->greenShift), 1 );
  con->read( &(info->blueShift), 1 );

  con->read( info->padding, 3 );

  con->read( &(info->nameLength), 4 );
  info->nameLength = Swap32IfLE( info->nameLength );

  qWarning( "Width = %d, Height = %d", info->width, info->height );
  qWarning( "Bpp = %d, Depth = %d, Big = %d, True = %d",
	   info->bpp, info->depth, info->bigEndian, info->trueColor );
  qWarning( "RedMax = %d, GreenMax = %d, BlueMax = %d",
	   info->redMax, info->greenMax, info->blueMax );
  qWarning( "RedShift = %d, GreenShift = %d, BlueShift = %d",
	   info->redShift, info->greenShift,info-> blueShift );

  buf->resize( info->width, info->height );

  // Wait for desktop name
  qWarning( "Waiting for desktop name" );

  static QString statusMsg = tr( "Waiting for desktop name..." );
  emit status( statusMsg );

  currentState = AwaitingDesktopName;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotDesktopName() ) );
  con->waitForData( info->nameLength );
}

void KRFBDecoder::gotDesktopName()
{
  assert( info );
  assert( currentState == AwaitingDesktopName );

  qWarning( "Got desktop name" );

  disconnect( con, SIGNAL( gotEnoughData() ), 
	      this, SLOT( gotDesktopName() ) );

  char *buf = new char[ info->nameLength + 1 ];
  CHECK_PTR( buf );

  con->read( buf, info->nameLength );
  buf[ info->nameLength ] = '\0';
  info->name = buf;

  qWarning( "Desktop: %s", info->name.latin1() );

  delete buf;

  // Get the format we'll really use and tell the server
  decidePixelFormat();
  sendPixelFormat();
  sendAllowedEncodings();
  currentState = Idle;

  QString msg;
  msg = tr( "Connected to %1" );
  msg = msg.arg( info->name );
  emit status( msg );

  sendUpdateRequest( false );
}

void KRFBDecoder::decidePixelFormat()
{
  assert( info );

  if ( format )
    delete format;
  format = new KRFBPixelFormat;
  CHECK_PTR( format );

  // What depth do we want?
  //
  // We'll use the minimum of the remote and local depths, UNLESS an
  // eight bit session has been specifically requested by the user.
  int screenDepth = QPixmap::defaultDepth();
  int bestDepth = ( screenDepth > info->depth ) ? info->depth : screenDepth;
  int chosenDepth;

  if ( con->options()->colors256 )
    chosenDepth = 8;
  else
    chosenDepth = bestDepth;

  qWarning( "Screen depth=%d, server depth=%d, best depth=%d, " \
	   "eight bit %d, chosenDepth=%d",
	   screenDepth,
	   info->depth,
	   bestDepth, 
	   con->options()->colors256, chosenDepth );

  format->depth = chosenDepth;

  // If we're using the servers native depth
  if ( chosenDepth == info->depth ) {
    // Use the servers native format
    format->bpp = info->bpp;
    //    format->bigEndian = info->bigEndian;
    format->bigEndian = true;
    format->trueColor = info->trueColor;
    format->redMax = info->redMax;
    format->greenMax = info->greenMax;
    format->blueMax = info->blueMax;
    format->redShift = info->redShift;
    format->greenShift = info->greenShift;
    format->blueShift = info->blueShift;
  }
  else {
    if ( chosenDepth == 8 ) {
      format->bpp = 8;
      format->bigEndian = true;
      format->trueColor = true;
      format->redMax = 7;
      format->greenMax = 7;
      format->blueMax = 3;
      format->redShift = 0;
      format->greenShift = 3;
      format->blueShift = 6;
    }
  }

  format->redMax = Swap16IfLE( format->redMax );
  format->greenMax = Swap16IfLE( format->greenMax );
  format->blueMax = Swap16IfLE( format->blueMax );
}

void KRFBDecoder::sendPixelFormat()
{
  static char padding[3];
  con->write( &SetPixelFormatId, 1 );
  con->write( padding, 3 );

  con->write( &(format->bpp), 1 );
  con->write( &(format->depth), 1 );
  con->write( &(format->bigEndian), 1 );
  con->write( &(format->trueColor), 1 );

  con->write( &(format->redMax), 2 );
  con->write( &(format->greenMax), 2 );
  con->write( &(format->blueMax), 2 );

  con->write( &(format->redShift), 1 );
  con->write( &(format->greenShift), 1 );
  con->write( &(format->blueShift), 1 );
  con->write( format->padding, 3 ); // Padding
}

void KRFBDecoder::sendAllowedEncodings()
{
  static CARD8 padding[1];
  con->write( &SetEncodingsId, 1 );
  con->write( padding, 1 );

  static CARD16 noEncodings = con->options()->encodings();
  noEncodings = Swap16IfLE( noEncodings );
  con->write( &noEncodings, 2 );

  if ( con->options()->corre )
    con->write( &CorreEncoding, 4 );
  if ( con->options()->hexTile )
    con->write( &HexTileEncoding, 4 );
  if ( con->options()->rre )
    con->write( &RreEncoding, 4 );
  if ( con->options()->copyrect )
    con->write( &CopyRectEncoding, 4 );
  // We always support this
  con->write( &RawEncoding, 4 );
}

void KRFBDecoder::sendUpdateRequest( bool incremental )
{
  if ( currentState != Idle )
    return;

  con->write( &UpdateRequestId, 1 );
  con->write( &incremental, 1 );

  static CARD16 x = 0, y = 0;
  static CARD16 w = Swap16IfLE( info->width );
  static CARD16 h = Swap16IfLE( info->height );

  con->write( &x, 2 );
  con->write( &y, 2 );
  con->write( &w, 2 );
  con->write( &h, 2 );

  // Now wait for the update
  currentState = AwaitingUpdate;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotUpdateHeader() ) );
  con->waitForData( UpdateHeaderLength );
}

void KRFBDecoder::gotUpdateHeader()
{
  assert( currentState == AwaitingUpdate );

  //  qWarning( "Got update header" );

  disconnect( con, SIGNAL( gotEnoughData() ), 
	      this, SLOT( gotUpdateHeader() ) );

  CARD8 msgType;
  con->read( &msgType, 1 );

  if ( msgType != UpdateId ) {
    // We might have a bell or server cut
    if ( msgType == ServerCutId ) {
      oldState = currentState;
      gotServerCut();
    }
    else if ( msgType == BellId ) {
      oldState = currentState;
      gotBell();
    }
    else {
      int msg = msgType;
      QString protocolError = tr( "Protocol Error: Message Id %1 was "
                                    "found when expecting an update "
                                    "message." ).arg( msg );
      currentState = Error;
      emit error( protocolError );
    }
    return;    
  }

  CARD8 padding;
  con->read( &padding, 1 );

  con->read( &noRects, 2 );
  noRects = Swap16IfLE( noRects );

  //  qWarning( "Expecting %d rects", noRects );

  // Now wait for the data
  currentState = AwaitingRectHeader;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotRectHeader() ) );
  con->waitForData( RectHeaderLength );
}

void KRFBDecoder::gotRectHeader()
{
  assert( currentState == AwaitingRectHeader );

  //  qWarning( "Got rect header" );

  disconnect( con, SIGNAL( gotEnoughData() ), 
	      this, SLOT( gotRectHeader() ) );

  con->read( &x, 2 );
  x = Swap16IfLE( x );
  con->read( &y, 2 );
  y = Swap16IfLE( y );

  con->read( &w, 2 );
  w = Swap16IfLE( w );
  con->read( &h, 2 );
  h = Swap16IfLE( h );

  con->read( &encoding, 4 );

  //  CARD32 encodingLocal = Swap32IfLE( encoding );
  //  qWarning( "Rect: x=%d, y= %d, w=%d, h=%d, encoding=%ld", 
  //	   x, y, w, h, encodingLocal );

  //
  // Each encoding needs to be handled differently. Some require
  // waiting for more data, but others like a copyrect do not.
  // Our constants have already been byte swapped, so we use
  // the remote value as is.
  //
  if ( encoding == RawEncoding ) {
      //    qWarning( "Raw encoding" );
    handleRawRect();
  }
  else if ( encoding == CopyRectEncoding ) {
//    qWarning( "CopyRect encoding" );
    handleCopyRect();
  }
  else if ( encoding == RreEncoding ) {
    qWarning( "RRE encoding" );
    handleRRERect();
  }
  else if ( encoding == CorreEncoding ) {
    qWarning( "CoRRE encoding" );
    handleCoRRERect();
  }
  else if ( encoding == HexTileEncoding ) {
    qWarning( "HexTile encoding" );
    handleHexTileRect();
  }
  else {
    int msg = Swap32IfLE( encoding );
    QString protocolError = tr( "Protocol Error: An unknown encoding was "
				  "used by the server %1" ).arg( msg );
    currentState = Error;
    qWarning( "Unknown encoding, %d", msg );
    emit error( protocolError );
    return;
  }
}

//
// Raw Encoding
//

void KRFBDecoder::handleRawRect()
{
  // We need something a bit cleverer here to handle large
  // rectanges nicely. The chunking should be based on the
  // overall size (but has to be in complete lines).

    //  qWarning( "Handling a raw rect chunk" );

    //  CARD32 lineCount = w * format->bpp / 8;

  if ( h > RectChunkSize ) {
    //    if ( con->sock->size() / lineCount ) {
    //      getRawRectChunk( con->sock->size() / lineCount );
    //    }
    //    else {
      getRawRectChunk( RectChunkSize );
      //   }
  }
  else {
    getRawRectChunk( h );
  }
}

void KRFBDecoder::getRawRectChunk( int lines )
{
  this->lines = lines;
  CARD32 count = lines * w * format->bpp / 8;

  // Wait for server init
  //  qWarning( "Waiting for raw rect chunk, %ld", count );

  currentState = AwaitingRawRectChunk;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotRawRectChunk() ) );
  con->waitForData( count );
}

void KRFBDecoder::gotRawRectChunk()
{
  assert( currentState == AwaitingRawRectChunk );

  disconnect( con, SIGNAL( gotEnoughData() ), 
              this, SLOT( gotRawRectChunk() ) );

  //  qWarning( "Got raw rect chunk" );

  //
  // Read the rect data and copy it to the buffer.
  //
  
  // TODO: Replace this!
  int count = lines * w * format->bpp / 8;
  char *hack = new char[ count ];
  con->read( hack, count );
  buf->drawRawRectChunk( hack, x, y, w, lines );
  delete hack;
  // /TODO:

  h = h - lines;
  y = y + lines;

  if ( h > 0 ) {
    handleRawRect();
  }
  else {
    noRects--;

    //    qWarning( "There are %d rects left", noRects );

    if ( noRects ) {
      currentState = AwaitingRectHeader;
      connect( con, SIGNAL( gotEnoughData() ), SLOT( gotRectHeader() ) );
      con->waitForData( RectHeaderLength );
    }
    else
      currentState = Idle;
  }
}

//
// Copy Rectangle Encoding
//

void KRFBDecoder::handleCopyRect()
{
  currentState = AwaitingCopyRectPos;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotCopyRectPos() ) );
  con->waitForData( CopyRectPosLength );
}

void KRFBDecoder::gotCopyRectPos()
{
  disconnect( con, SIGNAL( gotEnoughData() ),
              this, SLOT( gotCopyRectPos() ) );

  CARD16 srcX;
  CARD16 srcY;

  con->read( &srcX, 2 );
  con->read( &srcY, 2 );

  srcX = Swap16IfLE( srcX );
  srcY = Swap16IfLE( srcY );

  buf->copyRect( srcX, srcY, x, y, w, h );

  noRects--;

  //    qWarning( "There are %d rects left", noRects );

  if ( noRects ) {
    currentState = AwaitingRectHeader;
    connect( con, SIGNAL( gotEnoughData() ), SLOT( gotRectHeader() ) );
    con->waitForData( RectHeaderLength );
  }
  else
    currentState = Idle;
}

void KRFBDecoder::handleRRERect()
{
  qWarning( "RRE not implemented" );
}

void KRFBDecoder::handleCoRRERect()
{
  qWarning( "CoRRE not implemented" );
}

void KRFBDecoder::handleHexTileRect()
{
  qWarning( "HexTile not implemented" );
}

void KRFBDecoder::sendMouseEvent( QMouseEvent *e )
{
  // Deal with the buttons
  if ( e->type() != QEvent::MouseMove ) {
    buttonMask = 0;
    if ( e->type() == QEvent::MouseButtonPress ) {
      if ( e->button() & LeftButton )
        buttonMask |= 0x01;
      if ( e->button() & MidButton )
        buttonMask |= 0x04;
      if ( e->button() & RightButton )
        buttonMask |= 0x02;
    }
    else if ( e->type() == QEvent::MouseButtonRelease ) {
      if ( e->button() & LeftButton )
        buttonMask &= 0x06;
      if ( e->button() & MidButton )
        buttonMask |= 0x03;
      if ( e->button() & RightButton )
        buttonMask |= 0x05;
    }
  }

  CARD16 x = Swap16IfLE( e->x() );
  CARD16 y = Swap16IfLE( e->y() );

  con->write( &PointerEventId, 1 );
  con->write( &buttonMask, 1 );
  con->write( &x, 2 );
  con->write( &y, 2 );
}


void KRFBDecoder::sendCutEvent( const QString &unicode )
{
  //
  // Warning: There is a bug in the RFB protocol because there is no way to find
  // out the codepage in use on the remote machine. This could be fixed by requiring
  // the remote server to use utf8 etc. but for now we have to assume they're the
  // same. I've reported this problem to the ORL guys, but they apparantly have no
  // immediate plans to fix the issue. :-( (rich)
  //

  CARD8 padding[3];
  QCString text = unicode.local8Bit();
  CARD32 length = text.length();
  length = Swap32IfLE( length );

  con->write( &ClientCutTextId, 1 );
  con->write( &padding, 3 );
  con->write( &length, 4 );
  con->write( text.data(), length );
}

void KRFBDecoder::gotServerCut()
{
  qWarning( "Got server cut" );

  currentState = AwaitingServerCutLength;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotServerCutLength() ) );
  con->waitForData( ServerCutLenLength );
}

void KRFBDecoder::gotServerCutLength()
{
  assert( currentState = AwaitingServerCutLength );
  disconnect( con, SIGNAL( gotEnoughData() ),
              this, SLOT( gotServerCutLength() ) );

  CARD8 padding[3];
  con->read( padding, 3 );

  con->read( &serverCutTextLen, 4 );
  serverCutTextLen = Swap32IfLE( serverCutTextLen );

  currentState = AwaitingServerCutText;
  connect( con, SIGNAL( gotEnoughData() ), SLOT( gotServerCutText() ) );
  con->waitForData( serverCutTextLen );
}

void KRFBDecoder::gotServerCutText()
{
  assert( currentState = AwaitingServerCutText );

  disconnect( con, SIGNAL( gotEnoughData() ),
              this, SLOT( gotServerCutText() ) );

  //
  // Warning: There is a bug in the RFB protocol because there is no way to find
  // out the codepage in use on the remote machine. This could be fixed by requiring
  // the remote server to use utf8 etc. but for now we have to assume they're the
  // same. I've reported this problem to the ORL guys, but they apparantly have no
  // immediate plans to fix the issue. :-( (rich)
  //

  char *cutbuf = new char[ serverCutTextLen + 1 ];
  CHECK_PTR( cutbuf );

  con->read( cutbuf, serverCutTextLen );
  cutbuf[ serverCutTextLen ] = '\0';

  qWarning( "Server cut: %s", cutbuf );

  QString cutText( cutbuf ); // DANGER!!
  qApp->clipboard()->setText( cutText );

  delete cutbuf;

  // Now wait for the update (again)
  if ( oldState == AwaitingUpdate ) {
    currentState = AwaitingUpdate;
    connect( con, SIGNAL( gotEnoughData() ), SLOT( gotUpdateHeader() ) );
    con->waitForData( UpdateHeaderLength );
  }
  else if ( oldState == Idle ) {
    currentState = Idle;
  }
  else {
    qWarning( "Async handled in weird state" );
    currentState = oldState;
  };
}

void KRFBDecoder::gotBell()
{
  qWarning( "Got server bell" );
  buf->soundBell();

  // Now wait for the update (again)
  if ( oldState == AwaitingUpdate ) {
    currentState = AwaitingUpdate;
    connect( con, SIGNAL( gotEnoughData() ), SLOT( gotUpdateHeader() ) );
    con->waitForData( UpdateHeaderLength );
  }
  else if ( oldState == Idle ) {
    currentState = Idle;
  }
  else {
    qWarning( "Async handled in weird state" );
    currentState = oldState;
  };
}

void KRFBDecoder::sendKeyPressEvent( QKeyEvent *event )
{
  int key;
  key = toKeySym( event );
  if ( key ) {
    key = Swap32IfLE( key );

    CARD8 mask = true;

    CARD16 padding = 0;
    con->write( &KeyEventId, 1 );
    con->write( &mask, 1 );
    con->write( &padding, 2 );
    con->write( &key, 4 );
  }
}

void KRFBDecoder::sendKeyReleaseEvent( QKeyEvent *event )
{
  int key;
  key = toKeySym( event );
  if ( key ) {
    key = Swap32IfLE( key );

    CARD8 mask = false;

    CARD16 padding = 0;
    con->write( &KeyEventId, 1 );
    con->write( &mask, 1 );
    con->write( &padding, 2 );
    con->write( &key, 4 );
  }
}


int KRFBDecoder::toKeySym( QKeyEvent *k )
{
  int ke = 0;

  ke = k->ascii();
  // Markus: Crappy hack. I dont know why lower case letters are
  // not defined in qkeydefs.h. The key() for e.g. 'l' == 'L'. 
  // This sucks. :-(

  if ( (ke == 'a') || (ke == 'b') || (ke == 'c') || (ke == 'd') 
       || (ke == 'e') || (ke == 'f') || (ke == 'g') || (ke == 'h')
       || (ke == 'i') || (ke == 'j') || (ke == 'k') || (ke == 'l')
       || (ke == 'm') || (ke == 'n') || (ke == 'o') || (ke == 'p')
       || (ke == 'q') || (ke == 'r') || (ke == 's') || (ke == 't')
       || (ke == 'u') || (ke == 'v') ||( ke == 'w') || (ke == 'x')
       || (ke == 'y') || (ke == 'z') ) {
    ke = k->key();
    ke = ke + 0x20;
    return ke;
  }

  // qkeydefs = xkeydefs! :-)
  if ( ( k->key() >= 0x0a0 ) && k->key() <= 0x0ff )
      return k->key();

  if ( ( k->key() >= 0x20 ) && ( k->key() <= 0x7e ) )
    return k->key();
  
  // qkeydefs != xkeydefs! :-(
  // This is gonna suck :-(

  int i = 0;
  while ( keyMap[i].keycode ) {
    if ( k->key() == keyMap[i].keycode )
	return keyMap[i].keysym;
    i++;
  }

  return 0;
}

