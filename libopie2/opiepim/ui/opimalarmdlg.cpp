#include "opimalarmdlg.h"

#include <opie2/oresource.h>
#include <opie2/odebug.h>

#include <qpe/timestring.h>

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qtextview.h>
#include <qapplication.h>

#include <stdlib.h>

OPimAlarmDlg::OPimAlarmDlg( const QDateTime &eventTime, const QString &title, const QString &desc, int defaultSnooze, int defaultSnoozeUnits, bool ampm, bool viewEnabled, bool largeDesc, QWidget *parent )
    : OPimAlarmDlgBase( parent, 0, TRUE, WStyle_Dialog | WStyle_NoBorder | WStyle_StaysOnTop )
{
    m_eventTime = eventTime;
    m_response = None;
    m_ampm = ampm;

    if( largeDesc ) {
        fraTop->hide();
        lblDesc->hide();
        tvDesc->setText( desc );
        lblTime->hide();
    }
    else {
        pixmap->setPixmap( Opie::Core::OResource::loadPixmap("clock/alarmbell") );
        lblTitle->setText( tr("Alarm") );
        tvDesc->hide();
        lblDesc->setText( desc );
        updateTime();
    }
    setCaption( title );
    sbSnoozeTime->setValue( defaultSnooze );
    cbSnoozeUnits->setCurrentItem( defaultSnoozeUnits );
    slotSnoozeUnitsChanged( defaultSnoozeUnits );

//    QFont f(font());
//    f.setPointSize((int)(f.pointSize() * 1.7));
//    setFont(f);

    connect( cmdSnooze, SIGNAL( clicked() ), this, SLOT( snoozeClicked() ) );
    connect( cmdDismiss, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cbSnoozeUnits, SIGNAL( activated(int) ), this, SLOT( slotSnoozeUnitsChanged(int) ));
    if( viewEnabled )
        connect( cmdView, SIGNAL( clicked() ), this, SLOT( viewClicked() ) );
    else
        cmdView->hide();

    if( ! largeDesc )
        startTimer( 60000 );

    setFixedHeight( qApp->desktop()->height() );
}

OPimAlarmDlg::~OPimAlarmDlg()
{
}

QDateTime OPimAlarmDlg::snoozeDateTime( int snooze, int snoozeUnits )
{
    QDateTime ret = QDateTime::currentDateTime();
    switch( snoozeUnits ) {
        case 0:
            ret = ret.addSecs( snooze * 60 );
            break;
        case 1:
            ret = ret.addSecs( snooze * 3600 );
            break;
        case 2:
            ret = ret.addDays( snooze );
            break;
        default:
            owarn << "snoozeDateTime: Invalid unit ID " << snoozeUnits << " specified!" << oendl;
    }
    return ret;
}

QDateTime OPimAlarmDlg::snoozeDateTime()
{
    return snoozeDateTime( sbSnoozeTime->value(), cbSnoozeUnits->currentItem() );
}

OPimAlarmDlg::AlarmResponse OPimAlarmDlg::response()
{
    return m_response;
}

void OPimAlarmDlg::viewClicked()
{
    m_response = View;
    accept();
}

void OPimAlarmDlg::snoozeClicked()
{
    m_response = Snooze;
    accept();
}

void OPimAlarmDlg::updateTime()
{
    QString timeStr = TimeString::dateString( m_eventTime, m_ampm ) + "<br>";

    int warn = QDateTime::currentDateTime().secsTo( m_eventTime );
    if( warn >= 0 )
        warn += 5; // help with slight delays

    warn /= 60;
    if( warn == 0 )
        timeStr += tr(" (now)");
    else {
        if(warn > 60 || warn < -60) {
            int mins = abs(warn) % 60;
            int hours = abs(warn) / 60;
            if( warn > 0 )
                timeStr += tr(" (in %1h %2m)").arg(hours).arg(mins);
            else
                timeStr += tr(" (%1h %2m ago)").arg(hours).arg(mins);
        }
        else {
            if( warn > 0 )
                timeStr += tr(" (in %1 mins)").arg(warn);
            else
                timeStr += tr(" (%1 mins ago)").arg(-warn);
        }
    }
    lblTime->setText( QString("<center>") + timeStr + QString("</center>") );
}

void OPimAlarmDlg::timerEvent( QTimerEvent* )
{
    updateTime();
}

void OPimAlarmDlg::slotSnoozeUnitsChanged( int index )
{
    if( index == 0 )
        sbSnoozeTime->setLineStep( 5 );
    else
        sbSnoozeTime->setLineStep( 1 );
}
