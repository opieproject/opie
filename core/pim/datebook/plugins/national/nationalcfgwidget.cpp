#include "nationalcfgwidget.h"

#include <opie2/odebug.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qheader.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qheader.h>
#include <qdir.h>

/*
 *  Constructs a NationalHolidayConfig which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
NationalHolidayConfigWidget::NationalHolidayConfigWidget( QWidget* parent,  const char* name, WFlags fl )
    : Opie::Datebook::HolidayPluginConfigWidget( parent, name, fl )
{
    if ( !name )
        setName( "NationalHolidayConfig" );
    resize( 206, 210 );
    setCaption( tr( "Select national config" ) );
    NationalHolidayConfigLayout = new QVBoxLayout( this );
    NationalHolidayConfigLayout->setSpacing( 2 );
    NationalHolidayConfigLayout->setMargin( 2 );

    m_headLabel = new QLabel( this, "m_headLabel" );
    m_headLabel->setText( tr( "Select one or more configs to load" ) );
    m_headLabel->setAlignment( int( QLabel::AlignCenter ) );
    NationalHolidayConfigLayout->addWidget( m_headLabel );

    m_Configlist = new QListView( this, "m_Configlist" );
    m_Configlist->addColumn( tr( "configfiles" ) );
    QWhatsThis::add(  m_Configlist, tr( "List of found xml-files which may load." ) );
    NationalHolidayConfigLayout->addWidget( m_Configlist );

    // signals and slots connections
    connect( m_Configlist, SIGNAL( clicked(QListViewItem*) ), this, SLOT( listItemClicked(QListViewItem*) ) );
    init();
}

void NationalHolidayConfigWidget::init()
{
    m_Configlist->header()->hide();
    m_Configlist->setSorting(-1);

    QString path = QPEApplication::qpeDir() + "etc/nationaldays";
    QDir dir( path, "*.xml" );
    QStringList list = dir.entryList();
    QStringList::Iterator it;
    Config cfg("nationaldays");
    cfg.setGroup("entries");
    files = cfg.readListEntry("files");
    QCheckListItem *pitem = 0;

    for (it=list.begin();it!=list.end();++it) {
        pitem = new QCheckListItem(m_Configlist,(*it),QCheckListItem::CheckBox);
        if (files.find((*it)) != files.end()) {
            pitem->setOn(true);
        } else {
            pitem->setOn(false);
        }
    }
}

/*
 *  Destroys the object and frees any allocated resources
 */
NationalHolidayConfigWidget::~NationalHolidayConfigWidget()
{
    // no need to delete child widgets, Qt does it all for us
}

void NationalHolidayConfigWidget::listItemClicked(QListViewItem*item)
{
    if (!item) return;
    QCheckListItem*pitem = ((QCheckListItem*)item);
    QStringList::Iterator it = files.find(pitem->text(0));
    if (pitem->isOn()&&it==files.end()) {
        files.append(pitem->text(0));
    } else if (!pitem->isOn() && it!=files.end()) {
        files.remove(it);
    }
}

void NationalHolidayConfigWidget::saveConfig()
{
    Config cfg("nationaldays");
    cfg.setGroup("entries");
    cfg.writeEntry("files",files);
}

