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

#include "screenshot.h"
#include "inputDialog.h"

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <qpe/qpeapplication.h>
#include <qpe/applnk.h>

#include <qlineedit.h>
#include <qdir.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qimage.h>


/* XPM */
static char * snapshot_xpm[] = {
"32 32 177 2",
"  	c None",
". 	c #042045",
"+ 	c #0D2B47",
"@ 	c #0E325E",
"# 	c #0D2E50",
"$ 	c #0A1C32",
"% 	c #0F3A69",
"& 	c #164680",
"* 	c #165EAE",
"= 	c #134D89",
"- 	c #0A3A6E",
"; 	c #031024",
"> 	c #031B36",
", 	c #1A5EA3",
"' 	c #1862B1",
") 	c #1866B9",
"! 	c #0F5AAC",
"~ 	c #0F56A8",
"{ 	c #0C4C96",
"] 	c #030918",
"^ 	c #060206",
"/ 	c #20242C",
"( 	c #3E3B3B",
"_ 	c #186ABD",
": 	c #115EB3",
"< 	c #082644",
"[ 	c #222C38",
"} 	c #5A5859",
"| 	c #091921",
"1 	c #1E7EDE",
"2 	c #1A7ADA",
"3 	c #1970CD",
"4 	c #1758A1",
"5 	c #0E529A",
"6 	c #094388",
"7 	c #22364E",
"8 	c #384454",
"9 	c #04162C",
"0 	c #123451",
"a 	c #3296B4",
"b 	c #298AB1",
"c 	c #2484AC",
"d 	c #033D86",
"e 	c #033677",
"f 	c #072C58",
"g 	c #525862",
"h 	c #5E5E5E",
"i 	c #23492A",
"j 	c #4FBD3F",
"k 	c #50B24C",
"l 	c #51C72E",
"m 	c #42C64A",
"n 	c #2686CA",
"o 	c #0B52A4",
"p 	c #054A9A",
"q 	c #053B7C",
"r 	c #2B2325",
"s 	c #6E7070",
"t 	c #0D240E",
"u 	c #1E423E",
"v 	c #468ABE",
"w 	c #6A8AC2",
"x 	c #7EAA8E",
"y 	c #84BE4E",
"z 	c #65C639",
"A 	c #064EA0",
"B 	c #0A4DA0",
"C 	c #4E5242",
"D 	c #62362E",
"E 	c #721A1A",
"F 	c #6E0A0A",
"G 	c #3E0E0E",
"H 	c #6AB85E",
"I 	c #7E8ECA",
"J 	c #929CD4",
"K 	c #9EAECA",
"L 	c #9EBEAE",
"M 	c #8EC26A",
"N 	c #32A276",
"O 	c #20518C",
"P 	c #46566E",
"Q 	c #614A51",
"R 	c #AE2A26",
"S 	c #C41C1A",
"T 	c #CE0A0A",
"U 	c #A60606",
"V 	c #921616",
"W 	c #863735",
"X 	c #866A66",
"Y 	c #9E9EA2",
"Z 	c #1E5212",
"` 	c #7EC652",
" .	c #B2B2F2",
"..	c #CACAF6",
"+.	c #AECEC6",
"@.	c #92CE72",
"#.	c #42AA86",
"$.	c #365E96",
"%.	c #B82420",
"&.	c #E20E0E",
"*.	c #EF0E0B",
"=.	c #566A86",
"-.	c #92929A",
";.	c #16260E",
">.	c #2E571A",
",.	c #7ED64E",
"'.	c #9ECEB2",
").	c #C6D5EF",
"!.	c #E2E0FE",
"~.	c #A2DE8A",
"{.	c #4AAA5B",
"].	c #2A6AB2",
"^.	c #9A7A6E",
"/.	c #D25A52",
"(.	c #F62626",
"_.	c #FA0206",
":.	c #DE1E16",
"<.	c #B63A26",
"[.	c #865A2E",
"}.	c #26568E",
"|.	c #435F7D",
"1.	c #7AE246",
"2.	c #96EA6E",
"3.	c #BEEAAE",
"4.	c #D0EADC",
"5.	c #B6CEFE",
"6.	c #9ADE8A",
"7.	c #2A609A",
"8.	c #7E623E",
"9.	c #CA7A72",
"0.	c #F26E6E",
"a.	c #FE5252",
"b.	c #FA3638",
"c.	c #DA2E28",
"d.	c #9E3E3A",
"e.	c #3E7656",
"f.	c #76CB3A",
"g.	c #90DE44",
"h.	c #A2EE6A",
"i.	c #9EEA9A",
"j.	c #92BEDE",
"k.	c #7ADA7E",
"l.	c #5ECE52",
"m.	c #6A5E52",
"n.	c #A83238",
"o.	c #FA5656",
"p.	c #F29A92",
"q.	c #D29E92",
"r.	c #8A7A62",
"s.	c #05336B",
"t.	c #59C631",
"u.	c #8AD23A",
"v.	c #66E242",
"w.	c #1D6FC1",
"x.	c #E61212",
"y.	c #CA261E",
"z.	c #5A763E",
"A.	c #1E6E9A",
"B.	c #023272",
"C.	c #329696",
"D.	c #2074B6",
"E.	c #3E4E76",
"F.	c #5A525E",
"G.	c #425868",
"H.	c #04448E",
"I.	c #28828A",
"J.	c #2D8593",
"K.	c #12427A",
"L.	c #054696",
"M.	c #042B5E",
"N.	c #134F95",
"O.	c #1E6ABB",
"P.	c #1A5E96",
"Q.	c #022E67",
"R.	c #0C3E7C",
"S.	c #023E8A",
"T.	c #021A3E",
"                                                                ",
"                                                                ",
"                                                                ",
"            . + @ # +                                           ",
"      $ # % & * * = - # $                                       ",
"; > % @ = , ' ) ' ! ~ { % @ . ] ^ / (                           ",
"; + ' * ) _ ) ) ' : : ! { { % < . [ ( }                         ",
"| @ 1 2 3 _ _ ) ' : : : : 4 5 6 6 # 7 8 }                       ",
"9 0 a b b b c _ ) : : ! ! ~ ~ 5 ~ d e f [ 8 g h h }             ",
"| i j k j l m n 3 ) : ! ! ! ~ o o o p 6 q f / r r r s           ",
"t u v w x y y z a 1 3 ' : ! ~ A A B A p = C D E F G ( s         ",
"| u H I J K L M H k N 3 3 : ~ o o 5 O P Q R S T T U V W X Y     ",
"| Z ` J  .....+.@.z #.3 _ 3 3 ) $.P W %.S &.*.*.&.S W Q P =.-.  ",
";.>.,.'.).!.!.).~.,.{.) : ) 3 ].h ^./.(._._._.:.<.[.g }.= & |.  ",
";.>.1.2.3.4.4.5.6.,.{._ : : : 7.8.9.0.a.b.b.c.d.} e.4 = 6 q #   ",
";.>.z f.g.h.i.j.k.l.N ) : ! ! 4 m.n.c.o.p.q.r.|.5 { d e e s..   ",
"t Z l t.f.u.g.1.v.#.w.' : ! ~ ~ 7.|.n.x.y.z.A.A q - e e e B.>   ",
"| 0 c k t.t.l l C.D.) * * ! 5 { B o E.F.F.G.= H.e e e e q B..   ",
"; + D.I.J.I.I.I.w.) : : * o K.7 - L.= O = }.6 6 e q q e q s.>   ",
"9 + ) ) ) ) ' ' , * : ! ! ! K.< . M.q 6 B N.{ d q e e e e s.9   ",
"9 # O.) ) ' , , P.* ! ! ! ! = % . . M.e d p L.d B.B.e B.B.Q.9   ",
"; < ' ' ' ' , , 4 4 ~ ! ! ~ ! 5 e q e e q A H.d q q e e B.Q.9   ",
"; $ R.K.5 4 4 ' ! ! 4 ~ ! ~ ~ ~ o { B o A A L.S.B.B.B.B.B.Q.>   ",
"    ] $ 0 R.= ' ' 4 4 5 4 5 5 o B o B p A A L.d e e B.B.B.Q.9   ",
"          # + - { 4 4 ~ ! o { o L.p p p p p H.S.B.B.s.Q.Q.M.T.  ",
"                + s.6 B o o 5 B p L.p p L.p H.q B.Q.Q.Q.Q.M.;   ",
"                  < # s.- B o B p p L.L.H.L.H.d B.Q.Q.Q.Q.Q.9   ",
"                      $ . s.d 6 B A p H.S.L.H.q B.Q.Q.M.M.. ;   ",
"                          ; 9 . 6 L.p L.d L.H.d Q.M.M.. 9 ; ]   ",
"                                | > e L.d L.H.e M.. ; ] ]       ",
"                                    > 9 . S.Q.. ; ]             ",
"                                          T.; ]                 "};

static const char *SCAP_hostname = "www.handhelds.org";
static const int SCAP_port = 80;


ScreenshotControl::ScreenshotControl( QWidget *parent, const char *name )
    : QFrame( parent, name, WDestructiveClose | WStyle_StaysOnTop | WType_Popup )
{
  setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
  QVBoxLayout *vbox = new QVBoxLayout ( this, 5, 3 );
  QHBoxLayout *hbox;

  hbox = new QHBoxLayout ( vbox );

  QLabel *l = new QLabel ( tr( "Delay" ), this );
  hbox-> addWidget ( l );

  delaySpin = new QSpinBox( 0, 60, 1, this, "Spinner" );
  delaySpin-> setButtonSymbols ( QSpinBox::PlusMinus );
  delaySpin-> setSuffix ( tr( "sec" ));
  delaySpin-> setFocusPolicy( QWidget::NoFocus );
  delaySpin-> setValue ( 1 );
  hbox-> addWidget ( delaySpin );

  saveNamedCheck = new QCheckBox ( tr( "Save named" ), this);
  saveNamedCheck-> setFocusPolicy ( QWidget::NoFocus );
  vbox->addWidget( saveNamedCheck);

  vbox-> addSpacing ( 3 );

  l = new QLabel ( tr( "Save screenshot as..." ), this );
  vbox-> addWidget ( l, AlignCenter );

  hbox = new QHBoxLayout ( vbox );

  grabItButton = new QPushButton( tr( "File" ), this, "GrabButton" );
  grabItButton ->setFocusPolicy( QWidget::TabFocus );
  hbox-> addWidget ( grabItButton );

  scapButton = new QPushButton( tr( "Scap" ), this, "ScapButton" );
  scapButton ->setFocusPolicy( QWidget::TabFocus );
  hbox-> addWidget ( scapButton );

  setFixedSize ( sizeHint ( ));
  setFocusPolicy ( QWidget::NoFocus );


  grabTimer = new QTimer ( this, "grab timer");
  
  connect ( grabTimer, SIGNAL( timeout ( )), this, SLOT( performGrab ( )));
  connect ( grabItButton, SIGNAL( clicked ( )), SLOT( slotGrab ( )));
  connect ( scapButton, SIGNAL( clicked ( )), SLOT( slotScap ( )));
}

void ScreenshotControl::slotGrab()
{
  buttonPushed = 1;
  hide();

  setFileName = FALSE;
  if ( saveNamedCheck->isChecked()) {
    setFileName = TRUE;
    InputDialog *fileDlg;

    fileDlg = new InputDialog( 0 , tr("Name of screenshot "), TRUE, 0);
    fileDlg->exec();
    fileDlg->raise();
    QString fileName, list;
    if ( fileDlg->result() == 1 ) {
      fileName = fileDlg->LineEdit1->text();

      if (fileName.find("/", 0, TRUE) == -1)
        FileNamePath = QDir::homeDirPath() + "/Documents/image/png/" + fileName;
      else
        FileNamePath = fileName;

    }
    delete fileDlg;
  }

  if ( delaySpin->value() )
    grabTimer->start( delaySpin->value() * 1000, true );
  else
    show();
}

void ScreenshotControl::slotScap()
{
  buttonPushed = 2;
  hide();

  if ( delaySpin->value() )
    grabTimer->start( delaySpin->value() * 1000, true );
  else
    show();
}


void ScreenshotControl::savePixmap()
{
  DocLnk lnk;
  QString fileName;

  if ( setFileName) {
    fileName = FileNamePath;
    //not sure why this is needed here, but it forgets fileName
    // if this is below the braces

    if (fileName.right(3) != "png")
      fileName = fileName + ".png";
    lnk.setFile(fileName); //sets File property
    qDebug("saving file " + fileName);
    snapshot.save( fileName, "PNG");
    QFileInfo fi( fileName);
    lnk.setName( fi.fileName()); //sets file name

    if (!lnk.writeLink())
      qDebug("Writing doclink did not work");
  }
  else {

    fileName = "sc_" + QDateTime::currentDateTime().toString();
    fileName.replace(QRegExp("'"), "");
    fileName.replace(QRegExp(" "), "_");
    fileName.replace(QRegExp(":"), ".");
    fileName.replace(QRegExp(","), "");
    QString dirName = QDir::homeDirPath() + "/Documents/image/png/";

    if ( !QDir( dirName).exists() ) {
      qDebug("making dir " + dirName);
      QString msg = "mkdir -p " + dirName;
      system(msg.latin1());
    }
    fileName = dirName + fileName;
    if (fileName.right(3) != "png")
      fileName = fileName + ".png";
    lnk.setFile(fileName); //sets File property
    qDebug("saving file " + fileName);
    snapshot.save( fileName, "PNG");
    QFileInfo fi( fileName);
    lnk.setName( fi.fileName()); //sets file name

    if (!lnk.writeLink())
      qDebug("Writing doclink did not work");

  }

  QPEApplication::beep();
}

void ScreenshotControl::performGrab()
{
  snapshot = QPixmap::grabWindow( QPEApplication::desktop()->winId(), 0, 0, QApplication::desktop()->width(),  QApplication::desktop()->height() );

  if (buttonPushed == 1) {
    qDebug("grabbing screen");
    grabTimer->stop();
    show();
    qApp->processEvents();
    savePixmap();
  }
  else {
    grabTimer->stop();

    struct sockaddr_in raddr;
    struct hostent *rhost_info;
    int sock = -1;
    bool ok = false;

    if (( rhost_info = (struct hostent *) ::gethostbyname ((char *) SCAP_hostname )) != 0 ) {
      ::memset ( &raddr, 0, sizeof (struct sockaddr_in));
      ::memcpy ( &raddr. sin_addr, rhost_info-> h_addr, rhost_info-> h_length );
      raddr. sin_family = rhost_info-> h_addrtype;
      raddr. sin_port = htons ( SCAP_port );

      if (( sock = ::socket ( AF_INET, SOCK_STREAM, 0 )) >= 0 )
      {
        if ( ::connect ( sock, (struct sockaddr *) & raddr, sizeof (struct sockaddr)) >= 0 ) {
          QString header;

          QPixmap pix = ( snapshot.width() > snapshot.height() ) ? snapshot : snapshot.xForm( QWMatrix().rotate(90) );
          QImage img = pix.convertToImage().convertDepth( 16 ); // could make that also depth independent, if hh.org/scap can handle it

          header = "POST /scap/capture.cgi?%1+%2 HTTP/1.1\n"  // 1: model / 2: user
                   "Content-length: %3\n"                     // 3: content length
                   "Content-Type: image/gif\n"
                   "Host: %4\n"                               // 4: scap host
                   "\n";

          header = header.arg( "" ).arg( ::getenv ( "USER" ) ).arg( img.numBytes() ).arg( SCAP_hostname );

          if ( !pix.isNull() )
          {
            const char *ascii = header.latin1( );
            uint ascii_len = ::strlen( ascii );
            ::write ( sock, ascii, ascii_len );
            ::write ( sock, img.bits(), img.numBytes() );

            ok = true;
          }
        }
        ::close ( sock );
      }
    }
    if ( ok )
      QMessageBox::information( 0, tr( "Success" ), QString( "<p>%1</p>" ).arg ( tr( "Screenshot was uploaded to %1" )).arg( SCAP_hostname ));
    else
      QMessageBox::warning( 0, tr( "Error" ), QString( "<p>%1</p>" ).arg( tr( "Connection to %1 failed." )).arg( SCAP_hostname ));
  }

}



//===========================================================================

ScreenshotApplet::ScreenshotApplet( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
		setFixedWidth( AppLnk::smallIconSize());

    QImage  img = (const char **)snapshot_xpm;
    img = img.smoothScale(AppLnk::smallIconSize(), AppLnk::smallIconSize());
    m_icon.convertFromImage(img);
}

ScreenshotApplet::~ScreenshotApplet()
{
}

void ScreenshotApplet::mousePressEvent( QMouseEvent *)
{
  ScreenshotControl *sc = new ScreenshotControl ( );
  QPoint curPos = mapToGlobal ( QPoint ( 0, 0 ));

  // windowPosX is the windows position centered above the applets icon.
  // If the icon is near the edge of the screen, the window would leave the visible area
  // so we check the position against the screen width and correct the difference if needed
  
  int screenWidth = qApp->desktop()->width();
  int windowPosX = curPos. x ( ) - ( sc-> sizeHint ( ). width ( ) - width ( )) / 2 ;
  int ZwindowPosX, XwindowPosX;
  
  // the window would be placed beyond the screen wich doesn't look tooo good
  if ( (windowPosX + sc-> sizeHint ( ). width ( )) > screenWidth ) {
  	XwindowPosX = windowPosX + sc-> sizeHint ( ). width ( ) - screenWidth;
	ZwindowPosX = windowPosX - XwindowPosX - 1;
  } else {
  	ZwindowPosX = windowPosX;
  }
  
   sc-> move ( ZwindowPosX, curPos. y ( ) - sc-> sizeHint ( ). height ( ) );
   sc-> show ( );
}

void ScreenshotApplet::paintEvent( QPaintEvent* )
{
  QPainter p ( this );
	p.drawPixmap( 0,0, m_icon );
}

