/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@handhelds.org>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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
#include <qpushbutton.h>

#include "view.h"

using namespace Opie::Datebook;

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

    connect(locButton, SIGNAL( clicked() ), this, SLOT( slotConfigureLocs() ) );
    connect(descButton, SIGNAL( clicked() ), this, SLOT( slotConfigureDesc() ) );
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

void DateBookSettings::setViews( QList<Opie::Datebook::View> *views )
{
    m_views = views;
    for ( QListIterator<View> it(*m_views); it.current(); ++it ) {
        QWidget *widget = it.current()->createConfigWidget( viewstab );
        if( widget )
            viewstabLayout->addWidget( widget );
        m_viewConfigs.append( widget ); // must add even if null
        comboDefaultView->insertItem( it.current()->name() );
    }    
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    viewstabLayout->addItem( spacer_3 );
}

void DateBookSettings::saveViews()
{
    int i=0;
    for ( QListIterator<View> it(*m_views); it.current(); ++it ) {
        QWidget *widget = m_viewConfigs.at(i);
        if(widget)
            it.current()->readConfigWidget( widget );
        i++;
    }    
}

void DateBookSettings::setManagers( const DescriptionManager &descMan, const LocationManager &locMan )
{
    m_descMan = descMan;
    m_locMan = locMan;
}

DescriptionManager DateBookSettings::descriptionManager() const
{
    return m_descMan;
}

LocationManager DateBookSettings::locationManager() const
{
    return m_locMan;
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
        } 
        else {
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
    } 
    else {
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
    } 
    else if ( ampm && spinStart->suffix().contains( tr("PM"), FALSE )  ) {
        if ( saveMe != 12 )
            saveMe += 12;
    }
    ampm = whichClock;
    init();
    setStartTime( saveMe );
}

void DateBookSettings::slotConfigureDesc() {
    DescriptionManagerDialog dlg( m_descMan );
    dlg.setCaption( tr("Configure Descriptions") );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
        m_descMan = dlg.manager();
    }
}

void DateBookSettings::slotConfigureLocs() {
    LocationManagerDialog dlg( m_locMan );
    dlg.setCaption( tr("Configure Locations") );
    if ( QPEApplication::execDialog( &dlg ) == QDialog::Accepted ) {
        m_locMan = dlg.manager();
    }
}

