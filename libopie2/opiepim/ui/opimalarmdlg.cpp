#include "opimalarmdlg.h"

#include <opie2/oresource.h>
#include <opie2/odebug.h>

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpixmap.h>

OPimAlarmDlg::OPimAlarmDlg( int uid, const QDateTime &alarmTime, const QString &title, const QString &desc, int defaultSnooze, int defaultSnoozeUnits, bool viewEnabled, QWidget *parent, bool modal )
    : OPimAlarmDlgBase( parent, 0, modal )
{
    m_alarmTime = alarmTime;
    m_uid = uid;
    m_snooze = FALSE;
    
    pixmap->setPixmap( Opie::Core::OResource::loadPixmap("clock/alarmbell") );

    lblTitle->setText( title ); 
    lblDesc->setText( desc );
    sbSnoozeTime->setValue( defaultSnooze );
    cbSnoozeUnits->setCurrentItem( defaultSnoozeUnits );

    connect( cmdSnooze, SIGNAL( clicked() ), this, SLOT( snoozeClicked() ) );
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

bool OPimAlarmDlg::snooze()
{
    return m_snooze;
}

void OPimAlarmDlg::viewClicked()
{
    accept();
    emit viewItem( m_uid );
}

void OPimAlarmDlg::snoozeClicked()
{
    m_snooze = TRUE;
    accept();
}
