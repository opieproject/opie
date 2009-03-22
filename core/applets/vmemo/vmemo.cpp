/*
                             This file is part of the Opie Project
              =.             Copyright 2009 Team Opie <opie@handhelds.org>
            .=l.             Copyright 2002 Jeremy Cowgar <jc@cowgar.com>
           .>+-=             Copyright 2002 and 2003 L.J.Potter <ljp@llornkcor.com>
 _;:,     .>    :=|.         
.> <`_,   >  .   <=          This program is free software; you can
:`=1 )Y*s>-.--   :           redistribute it and/or  modify it under
.="- .-=="i,     .._         the terms of the GNU Library General Public
 - .   .-<_>     .<>         License as published by the Free Software
     ._= =}       :          Foundation; version 2 of the License,
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

#include "vmemo.h"

#include <opie2/otaskbarapplet.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qpainter.h>
#include <qmessagebox.h>

int seq = 0;

/* XPM */
static char * vmemo_xpm[] = {
    "16 16 102 2",
    "   c None",
    ".  c #60636A",
    "+  c #6E6E72",
    "@  c #68696E",
    "#  c #4D525C",
    "$  c #6B6C70",
    "%  c #E3E3E8",
    "&  c #EEEEF2",
    "*  c #EAEAEF",
    "=  c #CACAD0",
    "-  c #474A51",
    ";  c #171819",
    ">  c #9B9B9F",
    ",  c #EBEBF0",
    "'  c #F4F4F7",
    ")  c #F1F1F5",
    "!  c #DEDEE4",
    "~  c #57575C",
    "{  c #010101",
    "]  c #A2A2A6",
    "^  c #747477",
    "/  c #B5B5B8",
    "(  c #AEAEB2",
    "_  c #69696D",
    ":  c #525256",
    "<  c #181C24",
    "[  c #97979B",
    "}  c #A7A7AC",
    "|  c #B0B0B4",
    "1  c #C8C8D1",
    "2  c #75757B",
    "3  c #46464A",
    "4  c #494A4F",
    "5  c #323234",
    "6  c #909095",
    "7  c #39393B",
    "8  c #757578",
    "9  c #87878E",
    "0  c #222224",
    "a  c #414144",
    "b  c #6A6A6E",
    "c  c #020C16",
    "d  c #6B6B6F",
    "e  c #68686D",
    "f  c #5B5B60",
    "g  c #8A8A8F",
    "h  c #6B6B6E",
    "i  c #ADADB2",
    "j  c #828289",
    "k  c #3E3E41",
    "l  c #CFCFD7",
    "m  c #4C4C50",
    "n  c #000000",
    "o  c #66666A",
    "p  c #505054",
    "q  c #838388",
    "r  c #A1A1A7",
    "s  c #A9A9AE",
    "t  c #A8A8B0",
    "u  c #5E5E63",
    "v  c #3A3A3E",
    "w  c #BDBDC6",
    "x  c #59595E",
    "y  c #76767C",
    "z  c #373738",
    "A  c #717174",
    "B  c #727278",
    "C  c #1C1C1E",
    "D  c #3C3C3F",
    "E  c #ADADB6",
    "F  c #54555A",
    "G  c #8B8C94",
    "H  c #5A5A5F",
    "I  c #BBBBC3",
    "J  c #C4C4CB",
    "K  c #909098",
    "L  c #737379",
    "M  c #343437",
    "N  c #8F8F98",
    "O  c #000407",
    "P  c #2D3137",
    "Q  c #B0B1BC",
    "R  c #3B3C40",
    "S  c #6E6E74",
    "T  c #95959C",
    "U  c #74747A",
    "V  c #1D1D1E",
    "W  c #91929A",
    "X  c #42444A",
    "Y  c #22282E",
    "Z  c #B0B2BC",
    "`  c #898A90",
    " . c #65656A",
    ".. c #999AA2",
    "+. c #52535A",
    "@. c #151B21",
    "#. c #515257",
    "$. c #B5B5BE",
    "%. c #616167",
    "&. c #1A1D22",
    "*. c #000713",
    "=. c #1F1F21",
    "                                ",
    "            . + @ #             ",
    "          $ % & * = -           ",
    "        ; > , ' ) ! ~           ",
    "        { ] ^ / ( _ :           ",
    "        < [ } | 1 2 3           ",
    "      4 5 6 7 8 9 0 a b c       ",
    "    d e f g h i j 3 k l m n     ",
    "      o p q r s t u v w n       ",
    "      o x y z A B C D E n       ",
    "      F G H I J K L M N O       ",
    "      P Q R S T U V W X         ",
    "        Y Z ` b  ...+.          ",
    "          @.#.$.%.&.            ",
    "            *.B =.              ",
    "      n n n n n n n n n         "};


using namespace Opie::Ui;
VMemo::VMemo( QWidget *parent, const char *_name )
    : QWidget( parent, _name )
{
    setFixedHeight( 18 );
    setFixedWidth( 14 );

    t_timer = new QTimer( this );
    connect( t_timer, SIGNAL( timeout() ), SLOT( timerBreak() ) );

    myChannel = new QCopChannel( "QPE/VMemo", this );

    connect( myChannel, SIGNAL(received(const QCString&,const QByteArray&)),
            this, SLOT(receive(const QCString&,const QByteArray&)) );

    recording = false;

    readSettings();
}

VMemo::~VMemo()
{
}

int VMemo::position()
{
    return 1;
}

void VMemo::receive( const QCString &msg, const QByteArray &data )
{
    QDataStream stream( data, IO_ReadOnly );

    if (msg == "toggleRecord()")  {
        if (recording) {
            stopRecording();
        }
        else {
            startRecording();
        }
    }
    else if (msg == "reloadSettings()")  {
        readSettings();
    }
}

void VMemo::paintEvent( QPaintEvent* )
{
    QPainter p(this);
    p.drawPixmap( 0, 1,( const char** )  vmemo_xpm );
}

void VMemo::mousePressEvent( QMouseEvent * /*me*/) {

    if(!recording) {
        if(!startRecording() ){
            QMessageBox::critical(0, tr("vmemo"), tr("Recording aborted"));
        }
    }
    else {
        stopRecording();
    }
}

void VMemo::mouseReleaseEvent( QMouseEvent * )
{
}

bool VMemo::startRecording()
{
    QString date = TimeString::dateString( QDateTime::currentDateTime(),false,true);
    date.replace(QRegExp("'"),"");
    date.replace(QRegExp(" "),"_");
    date.replace(QRegExp(":"),"-");
    date.replace(QRegExp(","),"");

    if( m_useAlerts ) {
        msgLabel = new QLabel( 0, "alertLabel" );
        msgLabel->setText( tr("<B><P><font size=+2>VMemo-Recording</font></B><p>%1</p>").arg("vm_"+ date));
        msgLabel->show();
    }
    else
        msgLabel=0;

    Config config( "Vmemo" );
    config.setGroup( "System" );
    QString fileName = config.readEntry("RecLocation", QPEApplication::documentDir() );

    int s;
    s=fileName.find(':');
    if(s)
        fileName=fileName.right(fileName.length()-s-2);
    odebug << "pathname will be "+fileName << oendl;

    if( fileName.left(1).find('/') == -1)
        fileName="/"+fileName;
    if( fileName.right(1).find('/') == -1)
        fileName+="/";

    fileName += "vm_"+ date + ".wav";
    odebug << "filename is " + fileName << oendl;

    if( !m_recorder.setup( fileName, m_fileparams ) ) {
        QMessageBox::critical(0, "vmemo", "Could not start recording", "Abort");
        return false;
    }

    if( m_maxseconds > 0 )
        t_timer->start( m_maxseconds * 1000 );

    // FIXME error checking!
    recording = true;
    m_recorder.record( this );

    QArray<int> cats(1);
    cats[0] = config.readNumEntry("Category", 0);

    QString dlName("vm_");
    dlName += date;
    DocLnk l;
    l.setFile(fileName);
    l.setName(dlName);
    l.setType("audio/x-wav");
    l.setCategories(cats);
    l.writeLink();
    return true;
}

void VMemo::recorderCallback(const char *buffer, const int bytes, const int totalbytes, bool &stopflag)
{
    if( recording == false )
        stopflag = true;
    else
        qApp->processEvents();
}

void VMemo::stopRecording()
{
//    show();
    odebug << "Stopped recording" << oendl;
    recording = false;
    if( msgLabel ) {
        msgLabel->close();
        msgLabel=0;
        delete msgLabel;
    }
    t_timer->stop();
//    Config cfg("Vmemo");
//    cfg.setGroup("Defaults");
//     if( cfg.readNumEntry("hideIcon",0) == 1 )
//         hide();
}

int VMemo::setToggleButton(int tog)
{
    for( int i=0; i < 10;i++) {
        switch (tog) {
        case 0:
            return -1;
            break;
        case 1:
            return 0;
            break;
        case 2:
            return Key_F24; //was Escape
            break;
        case 3:
            return Key_Space;
            break;
        case 4:
            return Key_F12;
            break;
        case 5:
            return Key_F9;
            break;
        case 6:
            return Key_F10;
            break;
        case 7:
            return Key_F11;
            break;
        case 8:
            return Key_F13;
            break;
        };
    }
    return -1;
}

void VMemo::timerBreak()
{
    //stop
    stopRecording();
    QMessageBox::message("Vmemo",tr("<p>Vmemo recording stopped (over time limit)"));
}

void VMemo::readSettings()
{
    Config vmCfg("Vmemo");

    vmCfg.setGroup("System");
    m_useAlerts = vmCfg.readBoolEntry("Alert",1);

    vmCfg.setGroup("Record");
    m_fileparams.sampleRate = vmCfg.readNumEntry("SampleRate", 22050);
    m_fileparams.channels = vmCfg.readNumEntry("Stereo", 1) ? 2 : 1; // 1 = stereo(2), 0 = mono(1)
    if (vmCfg.readNumEntry("SixteenBit", 1)==1)
        m_fileparams.resolution = 16;
    else
        m_fileparams.resolution = 8;

    m_maxseconds = vmCfg.readNumEntry("SizeLimit", 30);

    vmCfg.setGroup("Defaults");
    bool useADPCM = vmCfg.readBoolEntry("use_ADPCM", 0);
    if( useADPCM ) 
        m_fileparams.format = WAVE_FORMAT_DVI_ADPCM;
    else
        m_fileparams.format = WAVE_FORMAT_PCM;

    int toggleKey = setToggleButton(vmCfg.readNumEntry("toggleKey", -1));
    owarn <<"VMemo toggleKey" << toggleKey << oendl;

    // FIXME this ought to be removed in favour of enhancing the
    // Opie button system to allow registering shortcut keys. In any case
    // you can bind a hardware button to record already.
    if( toggleKey != -1 ) {
        owarn << "Register key " << toggleKey << "" << oendl;

        QCopEnvelope e("QPE/Launcher", "keyRegister(int,QCString,QCString)");
        //           e << 4096; // Key_Escape
        //          e << Key_F5; //4148
        e << toggleKey;
        e << QCString("QPE/VMemo");
        e << QCString("toggleRecord()");

        usingIcon = false;
    }
    else
        usingIcon = true;

    if (!usingIcon)
        hide();
    else
        show();
}

EXPORT_OPIE_APPLET_v1( VMemo )

