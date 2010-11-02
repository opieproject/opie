/*
                             This file is part of the Opie Project
              =.             Copyright 2009 Team Opie <opie-users@lists.sourceforge.net>
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
#include <opie2/oresource.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>

#include <qpainter.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qlayout.h>

int seq = 0;

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
    m_pixnormal = Opie::Core::OResource::loadPixmap( "vmemo/record", Opie::Core::OResource::SmallIcon );
    m_pixrecording = Opie::Core::OResource::loadPixmap( "inline/stop", Opie::Core::OResource::SmallIcon );

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
    if(recording)
        p.drawPixmap( 0, 2, m_pixrecording );
    else
        p.drawPixmap( 0, 2, m_pixnormal );
}

void VMemo::mousePressEvent( QMouseEvent *me ) {
    if(!recording) {
        QPopupMenu * menu = new QPopupMenu( this );
        menu->insertItem( tr( "Start recording" ), 0 );
        menu->insertSeparator();
        menu->insertItem( tr( "Settings..." ), 1 );
        QPoint p = mapToGlobal( QPoint( 0, 0 ) );
        QSize s = menu->sizeHint();
        int opt = menu->exec( QPoint( p.x() + ( width() / 2 ) - ( s.width() / 2 ),
                                    p.y() - s.height() ), 0 );

        if ( opt == 0 ) {
            if(!startRecording() ){
                QMessageBox::critical(0, tr("vmemo"), tr("Recording aborted"));
            }
        }
        else if ( opt == 1 ) {
            // Show vmemo settings
            QCopEnvelope e("QPE/Application/sound", "raise()" );
        }

        delete menu;
    }
    else {
        stopRecording();
    }
}

void VMemo::mouseReleaseEvent( QMouseEvent * )
{
}

void VMemo::setupStatusWidget( QString fileTitle )
{
    m_statusWidget = new QWidget();
    QHBoxLayout *layout2 = new QHBoxLayout( m_statusWidget );
    layout2->setSpacing( 4 );
    layout2->setMargin( 2 );
    QVBoxLayout *layout = new QVBoxLayout( m_statusWidget );
    layout2->addLayout( layout );
    layout->setSpacing( 4 );
    layout->setMargin( 2 );

    QLabel *msgLabel = new QLabel( m_statusWidget, "alertLabel" );
    msgLabel->setText( fileTitle );
    layout->addWidget( msgLabel );

    m_timeLabel = new QLabel( m_statusWidget );
    m_timeLabel->setText( "<b>00:00</b>" ); // no tr
    layout->addWidget( m_timeLabel );

    QPushButton *pb = new QPushButton( m_statusWidget );
    pb->setPixmap( m_pixrecording );
    pb->setFixedSize( 20, 20 );
    connect( pb, SIGNAL( clicked() ), SLOT( stopRecording() ) );
    layout2->addWidget( pb );

    m_statusWidget->setCaption( tr("VMemo - Recording") );
    QWidget * desk = QApplication::desktop();
    m_statusWidget->show();
    m_statusWidget->move( desk->width()/2 - m_statusWidget->width()/2 ,
        desk->width()/2-m_statusWidget->height()/2 );
}

bool VMemo::startRecording()
{
    QString date = TimeString::dateString( QDateTime::currentDateTime(),false,true);
    date.replace(QRegExp("'"),"");
    date.replace(QRegExp(" "),"_");
    date.replace(QRegExp(":"),"-");
    date.replace(QRegExp(","),"");

    if( m_useAlerts ) {
        setupStatusWidget( "vm_"+ date ); // no tr
    }
    else {
        m_statusWidget = 0;
        m_timeLabel = 0;
    }

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

    if( m_maxseconds > 0 || m_statusWidget )
        t_timer->start( 1000 );

    // FIXME error checking!
    m_elapsedseconds = 0;
    recording = true;
    repaint();
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
    if( m_statusWidget ) {
        m_statusWidget->close();
        delete m_statusWidget;
        m_statusWidget = 0;
        m_timeLabel = 0;
    }
    t_timer->stop();
//    Config cfg("Vmemo");
//    cfg.setGroup("Defaults");
//     if( cfg.readNumEntry("hideIcon",0) == 1 )
//         hide();
    repaint();
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
    m_elapsedseconds++;
    if( m_timeLabel )
        m_timeLabel->setText( "<b>" + QString("").sprintf( "%.2d:%.2d", m_elapsedseconds / 60, m_elapsedseconds % 60 ) + "</b>" );
    if( m_maxseconds > 0 && m_elapsedseconds >= m_maxseconds ) {
        //stop
        stopRecording();
        QMessageBox::message("Vmemo",tr("<p>Vmemo recording stopped (over time limit)"));
    }
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

