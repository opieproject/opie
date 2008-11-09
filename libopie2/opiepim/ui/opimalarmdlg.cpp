#include "opimalarmdlg.h"

#include <opie2/oresource.h>
#include <opie2/odebug.h>

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpixmap.h>

OPimAlarmDlg::OPimAlarmDlg( const QDateTime &alarmTime, const QString &title, const QString &desc, int defaultSnooze, int defaultSnoozeUnits, bool viewEnabled, QWidget *parent, bool modal )
    : OPimAlarmDlgBase( parent, 0, modal )
{
    m_alarmTime = alarmTime;
    m_response = None;
    
    pixmap->setPixmap( Opie::Core::OResource::loadPixmap("clock/alarmbell") );

    setCaption( title );
    lblTitle->setText( tr("Alarm") ); 
    lblDesc->setText( desc );
    sbSnoozeTime->setValue( defaultSnooze );
    cbSnoozeUnits->setCurrentItem( defaultSnoozeUnits );

    QFont f(font());
    f.setPointSize((int)(f.pointSize() * 1.7));
    setFont(f);

    connect( cmdSnooze, SIGNAL( clicked() ), this, SLOT( snoozeClicked() ) );
    connect( cmdDismiss, SIGNAL( clicked() ), this, SLOT( accept() ) );
    if( viewEnabled ) 
        connect( cmdView, SIGNAL( clicked() ), this, SLOT( viewClicked() ) );
    else
        cmdView->hide();
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
