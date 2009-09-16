//
//
// C++ Implementation: $MODULE$
//
// Description:
//
//
// Author: Patrick S. Vogt <tille@handhelds.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "applnksearch.h"
#include "applnkitem.h"

#include <opie2/oresource.h>

#include <qpe/applnk.h>
#include <qpe/qpeapplication.h>


AppLnkSearch::AppLnkSearch(QListView* parent, QString name): SearchGroup(parent, name)
{
    _apps = 0;
    setPixmap( 0, Opie::Core::OResource::loadPixmap( "osearch/applications", Opie::Core::OResource::SmallIcon ) );
}


AppLnkSearch::~AppLnkSearch()
{
    delete _apps;
}


bool AppLnkSearch::load()
{
    _apps = new AppLnkSet(QPEApplication::qpeDir());
    return true;
}

int AppLnkSearch::search()
{
    QList<AppLnk> appList = _apps->children();

    for ( AppLnk *app = appList.first(); app != 0; app = appList.next() ){
        if ( (m_search.match( app->name() ) != -1)
            || (m_search.match(app->comment()) != -1)
            || (m_search.match(app->exec()) != -1) ) {
            insertItem( app );
        }
        else if ( searchFile( app ) )
            insertItem( app );
        qApp->processEvents( 100 );
    }
    return m_resultCount;
}

void AppLnkSearch::insertItem( void *rec )
{
    (void)new AppLnkItem( this, (AppLnk*)rec );
    m_resultCount++;
}

void AppLnkSearch::setSearch(QRegExp re)
{
    setOpen( false );
    SearchGroup::setSearch( re );
}

