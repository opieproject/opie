/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#include "datebooksettings.h"

#include <opie2/opluginloader.h>
#include <opie2/odebug.h>
#include <opie2/oholidaypluginif.h>
#include <opie2/oholidayplugin.h>
#include <opie2/oholidayplugincfgwidget.h>

#include <qpe/qpeapplication.h>

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qheader.h>
#include <qtabwidget.h>
#include <qlayout.h>

DateBookSettings::DateBookSettings( bool whichClock, QWidget *parent,
                                    const char *name, bool modal, WFlags fl )
    : DateBookSettingsBase( parent, name, modal, fl ),
      ampm( whichClock )
{
    init();
    QObject::connect( qApp, SIGNAL( clockChanged(bool) ), this, SLOT( slotChangeClock(bool) ) );
    QArray<int> categories;
    comboCategory->setCategories( categories, "Calendar", tr("Calendar") );
    m_loader = 0;
    m_manager = 0;
    m_PluginListView->header()->hide();
    m_PluginListView->setSorting(-1);
}

DateBookSettings::~DateBookSettings()
{
}

void DateBookSettings::setStartTime( int newStartViewTime )
{
    if ( ampm ) {
    if ( newStartViewTime >= 12 ) {
        newStartViewTime %= 12;
        if ( newStartViewTime == 0 )
        newStartViewTime = 12;
        spinStart->setSuffix( tr(":00 PM") );
    }
    else if ( newStartViewTime == 0 ) {
        newStartViewTime = 12;
        spinStart->setSuffix( tr(":00 AM") );
    }
    oldtime = newStartViewTime;
    }
    spinStart->setValue( newStartViewTime );
}

int DateBookSettings::startTime() const
{
    int returnMe = spinStart->value();
    if ( ampm ) {
    if ( returnMe != 12 && spinStart->suffix().contains(tr("PM"), FALSE) )
        returnMe += 12;
    else if (returnMe == 12 && spinStart->suffix().contains(tr("AM"), TRUE))
        returnMe = 0;
    }
    return returnMe;
}

void DateBookSettings::setPluginList(Opie::Core::OPluginManager*aManager,Opie::Core::OPluginLoader*aLoader)
{
    m_manager = aManager;
    m_loader = aLoader;
    if (!aManager||!aLoader) return;
    Opie::Core::OPluginItem::List inLst = m_loader->allAvailable(true);
    QCheckListItem *pitem = 0;

    for ( Opie::Core::OPluginItem::List::Iterator it = inLst.begin(); it != inLst.end(); ++it ) {
        pitem = new QCheckListItem(m_PluginListView,(*it).name(),QCheckListItem::CheckBox);
        pitem->setOn( (*it).isEnabled() );

        Opie::Datebook::HolidayPluginIf*hif = m_loader->load<Opie::Datebook::HolidayPluginIf>(*it,IID_HOLIDAY_PLUGIN);
        if (!hif) continue;
        Opie::Datebook::HolidayPlugin*pl = hif->plugin();
        if (!pl) continue;
        Opie::Datebook::HolidayPluginConfigWidget*cfg = pl->configWidget();
        if (!cfg) continue;
        QWidget * dtab = new QWidget(TabWidget,pl->description());
        QVBoxLayout*dlayout = new QVBoxLayout(dtab);
        dlayout->setMargin(2);
        dlayout->setSpacing(2);
        cfg->reparent(dtab,0,QPoint(0,0));
        dlayout->addWidget(cfg);
        TabWidget->insertTab(dtab,pl->description());

        m_cfgWidgets.append(cfg);
    }
}
void DateBookSettings::savePlugins()
{
    QValueList<Opie::Datebook::HolidayPluginConfigWidget*>::Iterator it;
    for (it=m_cfgWidgets.begin();it!=m_cfgWidgets.end();++it) {
        (*it)->saveConfig();
    }
}

void DateBookSettings::pluginItemClicked(QListViewItem *aItem)
{
    if (!aItem||!m_manager||!m_loader) return;
    QCheckListItem*pitem = ((QCheckListItem*)aItem);

    Opie::Core::OPluginItem::List lst = m_loader->allAvailable( true );
    for ( Opie::Core::OPluginItem::List::Iterator it = lst.begin(); it != lst.end(); ++it ) {
        if ( QString::compare( (*it).name() , pitem->text(0) ) == 0 ) {
            m_manager->setEnabled((*it),pitem->isOn());
            break;
        }
    }
}

void DateBookSettings::setAlarmPreset( bool bAlarm, int presetTime )
{
    chkAlarmPreset->setChecked( bAlarm );
    if ( presetTime >=5 )
    spinPreset->setValue( presetTime );
}

bool DateBookSettings::alarmPreset() const
{
    return chkAlarmPreset->isChecked();
}

int DateBookSettings::presetTime() const
{
    return spinPreset->value();
}


void DateBookSettings::slot12Hour( int i )
{
    if ( ampm ) {
    if ( spinStart->suffix().contains( tr("AM"), FALSE ) ) {
        if ( oldtime == 12 && i == 11 || oldtime == 11 && i == 12 )
        spinStart->setSuffix( tr(":00 PM") );
    } else {
        if ( oldtime == 12 && i == 11 || oldtime == 11 && i == 12 )
        spinStart->setSuffix( tr(":00 AM") );
    }
    oldtime = i;
    }
}

void DateBookSettings::init()
{
    if ( ampm ) {
    spinStart->setMinValue( 1 );
    spinStart->setMaxValue( 12 );
    spinStart->setValue( 12 );
    spinStart->setSuffix( tr(":00 AM") );
    oldtime = 12;
    } else {
    spinStart->setMinValue( 0 );
    spinStart->setMaxValue( 23 );
    spinStart->setSuffix( tr(":00") );
    }
}

void DateBookSettings::slotChangeClock( bool whichClock )
{
    int saveMe;
    saveMe = spinStart->value();
    if ( ampm && spinStart->suffix().contains( tr("AM"), FALSE ) ) {
    if ( saveMe == 12 )
        saveMe = 0;
    } else if ( ampm && spinStart->suffix().contains( tr("PM"), FALSE )  ) {
    if ( saveMe != 12 )
        saveMe += 12;
    }
    ampm = whichClock;
    init();
    setStartTime( saveMe );
}

void DateBookSettings::setJumpToCurTime( bool bJump )
{
    chkJumpToCurTime->setChecked( bJump );
}

bool DateBookSettings::jumpToCurTime() const
{
    return chkJumpToCurTime->isChecked();
}

void DateBookSettings::setRowStyle( int style )
{
    comboRowStyle->setCurrentItem( style );
}

int DateBookSettings::rowStyle() const
{
    return comboRowStyle->currentItem();
}
